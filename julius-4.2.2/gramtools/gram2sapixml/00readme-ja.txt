    gram2sapixml.pl

GRAM2SAPIXML.PL(1)                                          GRAM2SAPIXML.PL(1)



名前
           gram2sapixml.pl
          - 認識用文法を SAPI XML 文法に変換するスクリプト

概要
       gram2sapixml.pl [prefix...]

DESCRIPTION
       gram2sapixml.pl は，Julius の認識用文法ファイル (.grammar, .voca) から
       Microsoft SAPI XML 形式へ変換するスクリプトです． prefix には，変換する
       .grammar, .voca ファ イルのファイル名から拡張子を除外したものを指定しま
       す．複数指定した場合， それらは逐次変換されます．

       入力文字コードは EUC-JPを想定しています．出力ファイルは UTF-8 エンコー
       ディングです．コード変換のため内部で iconv を使用 しています．

       左再帰性については手作業による修正が必要です．元ファイルの .grammar の
       構造をそのまま保持するため，.grammar における正順での左再帰記述がその
       まま .xml に反映されます．したがって，変換後 .xml に含まれる左再帰性の
       解決は手作業で行わなければいけません．

SEE ALSO
        mkdfa.pl ( 1 )

DIAGNOSTICS
       変換は，元ファイルの文法の非終端記号と終端記号(単語カテゴリ名)をルール
       に変換するという単純なものです．実際にSAPIアプリケーションで使う場合に
       は，プロパティを指定するなど，手作業での修正が必要です．

       内部でコード変換に iconv を使用しています． 実行パス上に iconv が無い場
       合，エラーとなります．

COPYRIGHT
       Copyright (c) 2002  Takashi Sumiyoshi

LICENSE
       Julius の使用許諾に準じます．



                                  10/02/2008                GRAM2SAPIXML.PL(1)
