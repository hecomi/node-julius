#include <uv.h>
#include <iostream>
#include <memory>
#include "julius.hpp"

using namespace node;
using namespace v8;



/* ------------------------------------------------------------------------- */
//  グローバル変数 / データやりとり用 構造体
/* ------------------------------------------------------------------------- */

//! EventEmitter.emit 用のシンボル
static Persistent<String> emit_symbol;

//! Julius の別スレッドからのアクセス用
static uv_mutex_t m;

//! Emit でやりとりするデータ
struct EmitData
{
	Julius *_this;
	std::string msg;
	std::string result;
};



/* ------------------------------------------------------------------------- */
//  Static メンバ
/* ------------------------------------------------------------------------- */

void Julius::Init(Local<Object>& target)
{
	auto isolate = Isolate::GetCurrent();

	auto clazz = FunctionTemplate::New(isolate, Julius::New);

	clazz->SetClassName( String::NewFromUtf8(isolate, "Julius") );
	clazz->InstanceTemplate()->SetInternalFieldCount(1);

	clazz->PrototypeTemplate()->Set(
		String::NewFromUtf8(isolate, "start"),
		FunctionTemplate::New(isolate, Julius::Start)->GetFunction()
	);
	clazz->PrototypeTemplate()->Set(
		String::NewFromUtf8(isolate, "reload"),
		FunctionTemplate::New(isolate, Julius::Reload)->GetFunction()
	);
	clazz->PrototypeTemplate()->Set(
		String::NewFromUtf8(isolate, "stop"),
		FunctionTemplate::New(isolate, Julius::Stop)->GetFunction()
	);

	target->Set( String::NewFromUtf8(isolate, "Julius"), clazz->GetFunction() );
}


void Julius::New(const FunctionCallbackInfo<v8::Value>& args)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	String::Utf8Value jconf_path(args[0]);
	std::cout << *jconf_path << std::endl;
	auto _this = new Julius(*jconf_path);
	_this->Wrap( args.This() );

	args.GetReturnValue().Set( args.This() );
}


void Julius::Start(const FunctionCallbackInfo<v8::Value>& args)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	auto _this = ObjectWrap::Unwrap<Julius>( args.This() );

	switch (j_open_stream(_this->recog_, nullptr)) {
		case 0:
			break; // success
		case -1:
			_this->Emit("error", "Error in input stream");
			args.GetReturnValue().Set(false);
			return;
		case -2:
			_this->Emit("error", "Failed to begin input stream");
			args.GetReturnValue().Set(false);
			return;
	}

	auto req  = new uv_work_t;
	req->data = _this;

	_this->Ref(); // v8 に GC されないようにする
	uv_queue_work(
		uv_default_loop(),
		req,
		[](uv_work_t* req) {
			std::cout << "Start" << std::endl;
			auto _this = static_cast<Julius*>(req->data);
			_this->Emit("start");
			j_recognize_stream(_this->recog_);
		},
		(uv_after_work_cb)([](uv_work_t* req, int) {
			auto _this = static_cast<Julius*>(req->data);
			_this->Unref(); // v8 に GC を許可する
		})
	);

	args.GetReturnValue().Set(true);
};


void Julius::Stop(const FunctionCallbackInfo<v8::Value>& args)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	auto _this = ObjectWrap::Unwrap<Julius>( args.This() );
	j_close_stream(_this->recog_);
	_this->Emit("pause");

	args.GetReturnValue().SetUndefined();
};


void Julius::Reload(const FunctionCallbackInfo<v8::Value>& args)
{
	auto isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	String::Utf8Value jconf_path(args[0]);
	auto _this = ObjectWrap::Unwrap<Julius>( args.This() );
	j_close_stream(_this->recog_);
	_this->init(*jconf_path);
	_this->Emit("reload");

	args.GetReturnValue().SetUndefined();
};


void Julius::Emit(const std::string& msg, const std::string& result)
{
	// やり取りするデータを作詞
	auto req     = new uv_work_t;
	auto data    = new EmitData;
	data->_this  = this;
	data->msg    = msg;
	data->result = result;
	req->data    = data;

	// 次回のループの最初にメインスレッドからコールバックを実行
	uv_queue_work(
		uv_default_loop(),
		req,
		[](uv_work_t* req) {},
		(uv_after_work_cb)([](uv_work_s* req, int) {
			uv_mutex_lock(&m);

			auto isolate = Isolate::GetCurrent();
			HandleScope scope(isolate);

			auto data = static_cast<EmitData*>(req->data);
			auto symbol = Local<v8::String>::New(isolate, emit_symbol);
			auto emit_v = data->_this->handle(isolate)->Get(symbol);
			assert( emit_v->IsFunction() );
			auto emit = emit_v.As<Function>();

			TryCatch tc;
			Handle<v8::Value> argv[] = {
				String::NewFromUtf8(isolate, data->msg.c_str()),
				String::NewFromUtf8(isolate, data->result.c_str())
			};
			emit->Call(data->_this->handle(isolate), 2, argv);
			if ( tc.HasCaught() ) {
				FatalException(isolate, tc);
			}

			uv_mutex_unlock(&m);
		})
	);
}



/* ------------------------------------------------------------------------- */
//  non-Static メンバ
/* ------------------------------------------------------------------------- */

Julius::Julius(const std::string& jconf_path)
: recog_(nullptr)
{
	init(jconf_path);
}


Julius::~Julius()
{
	free_recog();
}


void Julius::init(const std::string& jconf_path)
{
	make_recog(jconf_path);

	// デバッグメッセージを消す
	j_disable_debug_message();
	j_disable_verbose_message();
}


void Julius::make_recog(const std::string& jconf_path)
{
	// 古い recog がある場合は破棄
	free_recog();

	// jconf を読み込む
	std::vector<char> path( jconf_path.begin(), jconf_path.end() );
	path.push_back('\0');
	auto jconf = j_config_load_file_new( &path[0] );
	if (jconf == nullptr) {
		Emit("error", "Error@j_config_load_file_new");
		return;
	}

	// 認識を行うインスタンスを生成
	recog_ = j_create_instance_from_jconf(jconf);
	if (recog_ == nullptr) {
		Emit("error", "Error@j_create_instance_from_jconf");
		return;
	}

	// マイク入力の初期化
	if (j_adin_init(recog_) == FALSE) {
		Emit("error", "Error@j_adin_init");
		return;
	}

	// コールバックを追加
	add_callbacks();
}


void Julius::free_recog()
{
	if (recog_ == nullptr) {
		return;
	}

	j_close_stream(recog_);

	j_jconf_free(recog_->jconf);
	recog_->jconf = nullptr;

	j_recog_free(recog_);
	recog_ = nullptr;
}


void Julius::add_callbacks()
{
	// コールバックを追加する
	// --------------------------------------------------------------------------------
	// 待機時 JavaScript 側のコールバック呼び出し
	add_callback(CALLBACK_EVENT_SPEECH_READY, [](Recog* recog, void* ptr) {
		auto _this = static_cast<Julius*>(ptr);
		_this->Emit("speechReady");
	}, this);

	// 発話開始時 JavaScript 側のコールバック呼び出し
	add_callback(CALLBACK_EVENT_SPEECH_START, [](Recog* recog, void* ptr) {
		auto _this = static_cast<Julius*>(ptr);
		_this->Emit("speechStart");
	}, this);

	// 発話終了時 JavaScript 側のコールバック呼び出し
	add_callback(CALLBACK_EVENT_SPEECH_STOP, [](Recog* recog, void* ptr) {
		auto _this = static_cast<Julius*>(ptr);
		_this->Emit("speechStop");
	}, this);

	// ポーズ時 JavaScript 側のコールバック呼び出し
	add_callback(CALLBACK_EVENT_PAUSE, [](Recog* recog, void* ptr) {
		auto _this = static_cast<Julius*>(ptr);
		_this->Emit("pause");
	}, this);

	// 結果が返された時の JavaScript 側のコールバック呼び出し
	add_callback(CALLBACK_RESULT, [](Recog* recog, void* ptr) {
		auto _this = static_cast<Julius*>(ptr);
		_this->on_result(recog);
	}, this);
}


void Julius::on_result(Recog* recog)
{
	// 結果を走査
	for (const RecogProcess *r = recog->process_list; r; r = r->next) {
		WORD_INFO *wargs = r->lm->winfo;

		// 仮説の数に応じてループ
		for (int n = 0; n < r->result.sentnum; ++n) {
			// Windows だと起こらないらしいが Ubuntu だとたまに起こる…
			if (r->result.sent == nullptr) break;

			auto s     = r->result.sent[n];
			auto seq   = s.word;
			int seqnum = s.word_num;

			// 認識結果の文章を取得
			std::string output;
			for (int i = 1; i < seqnum-1; ++i) {
				// result[n][i] = wargs->woutput[seq[i]];
				output += wargs->woutput[seq[i]];
			}

			// JavaScript のコールバックを呼ぶ
			Emit("result", output);
		}
	}
}


int Julius::add_callback(const int code, callback func, void* ptr)
{
	return callback_add(recog_, code, func, ptr);
}


bool Julius::delete_callback(const int id)
{
	return callback_delete(recog_, id);
}



/* ------------------------------------------------------------------------- */
//  node.js の addon 化
/* ------------------------------------------------------------------------- */

void init(Local<Object> target)
{
	auto isolate = Isolate::GetCurrent();
	assert( uv_mutex_init(&m) == 0 );
	emit_symbol.Reset( isolate, String::NewFromUtf8(isolate, "emit") );

	Julius::Init(target);
}

NODE_MODULE(julius, init)
