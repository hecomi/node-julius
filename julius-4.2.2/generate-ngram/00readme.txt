    generate-ngram

GENERATE-NGRAM(1)                                            GENERATE-NGRAM(1)



NAME
           generate-ngram
          - random sentence generator from N-gram

SYNOPSIS
       generate-ngram [options...] {binary_ngram}

DESCRIPTION
       generate-ngram is a tool to generate sentences randomly according to
       the given N-gram language model. The N-gram model file binary_ngram
       should be an binary format.

OPTIONS
        -n  num
          Number of sentences to generate (default: 10)

        -N
          Specify which length of N-gram to use (default: available max in the
          given model)

        -bos
          Beginning-of-sentence word (default: "<s>")

        -eos
          End-of-sentence word (default: "</s>")

        -ignore
          Specify a word to be supressed from output (default: "<UNK")

        -v
          Verbose output.

        -debug
          Debug output.

SEE ALSO
        julius ( 1 ) ,
        mkbingram ( 1 )

COPYRIGHT
       Copyright (c) 1997-2000 Information-technology Promotion Agency, Japan

       Copyright (c) 1991-2008 Kawahara Lab., Kyoto University

       Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and
       Technology

       Copyright (c) 2005-2008 Julius project team, Nagoya Institute of
       Technology

LICENSE
       The same as Julius.



                                  10/02/2008                 GENERATE-NGRAM(1)
