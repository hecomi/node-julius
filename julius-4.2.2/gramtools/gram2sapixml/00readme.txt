    gram2sapixml.pl

GRAM2SAPIXML.PL(1)                                          GRAM2SAPIXML.PL(1)



NAME
           gram2sapixml.pl
          - convert Julius grammar to SAPI XML grammar format

SYNOPSIS
       gram2sapixml.pl [prefix...]

DESCRIPTION
       gram2sapixml.pl converts a recognition grammar file of Julius
       (.grammar, .voca) to Microsoft SAPI XML grammar format.  prefix should
       be a file name of target grammar, excluding suffixes. If multiple
       argument is given, each will be process sequencialy in turn.

       The internal character set should be in UTF-8 format. By default
       gram2sapixml.pl assume input in EUC-JP encoding and tries to convert it
       to UTF-8 using iconv. You may want to disable this feature within the
       script.

       It will fail to convert a left recursive rule in the grammar. When
       fails, it will leave the source rules in the target .xml file, so you
       should modify the output manually to solve it.

SEE ALSO
        mkdfa.pl ( 1 )

DIAGNOSTICS
       The conversion procedure is somewhat dumb one, only converting the
       non-terminal symbols and terminal symbols (=word category name) into
       corresponding rules one by one. This is only a help tool, and you will
       need a manual inspection and editing to use it on a real SAPI
       application.

COPYRIGHT
       Copyright (c) 2002  Takashi Sumiyoshi

LICENSE
       The same as Julius.



                                  10/02/2008                GRAM2SAPIXML.PL(1)
