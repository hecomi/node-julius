{
	'variables': {
		'julius_ver'     : '4.4.2',
		'libjulius_path' : 'julius-<(julius_ver)/libjulius',
		'libsent_path'   : 'julius-<(julius_ver)/libsent',
	},
	'action': [
		'make',
	],
	'targets': [
		{
			'target_name'    : 'julius',
			'sources'        : [
				'src/julius.cpp',
			],
			'cflags'         : [
				'<!@(<(libjulius_path)/libjulius-config --cflags)',
				'<!@(<(libsent_path)/libsent-config --cflags)',
				'-std=c++11',
			],
			'include_dirs'   : [
				'<(libjulius_path)/include',
				'<(libsent_path)/include',
			],
			'link_settings': {
				'libraries': [
					'-L../<(libjulius_path)',
					'-L../<(libsent_path)',
					'<!@(<(libjulius_path)/libjulius-config --libs)',
					'<!@(<(libsent_path)/libsent-config --libs)',
				],
			},
			'conditions': [
				['OS=="mac"', {
					'xcode_settings' : {
						'OTHER_CFLAGS': [
							'<!@(<(libjulius_path)/libjulius-config --cflags)',
							'<!@(<(libsent_path)/libsent-config --cflags)',
							'-std=c++11',
						],
					},
				}],
			],
		},
	],
}
