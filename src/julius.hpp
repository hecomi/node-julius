#ifndef INCLUDE_JULIUS_HPP
#define INCLUDE_JULIUS_HPP

#include <string>
#include <vector>
#include <julius/juliuslib.h>
#include <node.h>
#include <node_object_wrap.h>

/**
 * Julius を扱うクラス
 */
class Julius : public node::ObjectWrap
{
public:
	//! イベントリスナに登録するコールバック型
	typedef void (*callback)(Recog*, void*);

	/**
	 *  コンストラクタ
	 *  @param[in] jconf jconf ファイルのパス
	 */
	Julius(const std::string& jconf_path);

	/**
	 *  Destroctor
	 */
	~Julius();

	/**
	 *  Node.js の世界へエクスポートする
	 *  @param[out] target  NODE_MODULES でエクスポートする関数の引数
	 */
	static void Init(v8::Local<v8::Object>& target);

private:
	/**
	 *  Julius の初期化を行う
	 *  @param[in] jconf jconf ファイルのパス
	 */
	void init(const std::string& jconf_path);

	/**
	 *  JavaScript 側で new された時に呼ばれる
	 *  @param[in] args  JavaScript 側から与えられる引数の取得や返り値のセット
	 */
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

	/**
	 *  JavaScript 側で定義したイベントリスナを呼ぶ
	 *  @param[in] msg   イベント名
	 *  @param[in] args  JavaScript 側から与えられる引数の取得や返り値のセット
	 */
	void Emit(const std::string& msg, const std::string& result = "");

	/**
	 *  Julius の音声認識を開始するメソッド
	 *  JavaScript 側で start すると呼ばれる
	 *  @param[in] args  JavaScript 側から与えられる引数の取得や返り値のセット
	 */
	static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);

	/**
	 *  Julius を終了するメソッド
	 *  JavaScript 側で close すると呼ばれる
	 *  @param[in] args  JavaScript 側から与えられる引数の取得や返り値のセット
	 */
	static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);

	/**
	 *  Julius の設定をリロードするメソッド
	 *  JavaScript 側で reload すると呼ばれる
	 *  @param[in] args  JavaScript 側から与えられる引数の取得や返り値のセット
	 */
	static void Reload(const v8::FunctionCallbackInfo<v8::Value>& args);

	/**
	 *  音声認識を行う Recog インスタンスを生成する
	 *  @param[in] jconf jconf ファイルのパス
	 */
	void make_recog(const std::string& jconf_path);

	/**
	 *  Recog インスタンスを破棄する
	 */
	void free_recog();

	/**
	 *  コールバック関数郡を追加する
	 */
	void add_callbacks();

	/**
	 *  コールバック関数を追加する
	 *  @param[in] code  コールバックID (e.g. CALLBACK_EVENT_SPEECH_READY)
	 *  @param[in] func  コールバック関数
	 *  @param[in] ptr   コールバック関数に伝える任意のデータ
	 *  @return          コールバックのID
	 */
	int add_callback(const int id, callback func, void* ptr = nullptr);

	/**
	 *  スピーチ準備完了時のコールバック
	 *  @param[in] recog  認識結果
	 */
	void on_result(Recog* recog);

	/**
	 *  コールバックを解除
	 *  @param[in] id コールバックのID
	 *  @return true:success, false:failed
	 */
	bool delete_callback(const int id);

	//! Julius の音声認識結果を扱うトップレベルインスタンス
	Recog* recog_;
};

#endif // INCLUDE_JULIUS_HPP
