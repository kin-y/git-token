begin_unit
begin_comment
comment|/*  * diff-delta.c: generate a delta between two buffers  *  * This code was greatly inspired by parts of LibXDiff from Davide Libenzi  * http://www.xmailserver.org/xdiff-lib.html  *  * Rewritten for GIT by Nicolas Pitre<nico@cam.org>, (C) 2005-2007  *  * This code is free software; you can redistribute it and/or modify  * it under the terms of the GNU General Public License version 2 as  * published by the Free Software Foundation.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_comment
comment|/* maximum hash entry list for the same hash bucket */
end_comment
begin_define
DECL|macro|HASH_LIMIT
define|#
directive|define
name|HASH_LIMIT
value|64
end_define
begin_define
DECL|macro|RABIN_SHIFT
define|#
directive|define
name|RABIN_SHIFT
value|23
end_define
begin_define
DECL|macro|RABIN_WINDOW
define|#
directive|define
name|RABIN_WINDOW
value|16
end_define
begin_decl_stmt
DECL|variable|T
specifier|static
specifier|const
name|unsigned
name|int
name|T
index|[
literal|256
index|]
init|=
block|{
literal|0x00000000
block|,
literal|0xab59b4d1
block|,
literal|0x56b369a2
block|,
literal|0xfdeadd73
block|,
literal|0x063f6795
block|,
literal|0xad66d344
block|,
literal|0x508c0e37
block|,
literal|0xfbd5bae6
block|,
literal|0x0c7ecf2a
block|,
literal|0xa7277bfb
block|,
literal|0x5acda688
block|,
literal|0xf1941259
block|,
literal|0x0a41a8bf
block|,
literal|0xa1181c6e
block|,
literal|0x5cf2c11d
block|,
literal|0xf7ab75cc
block|,
literal|0x18fd9e54
block|,
literal|0xb3a42a85
block|,
literal|0x4e4ef7f6
block|,
literal|0xe5174327
block|,
literal|0x1ec2f9c1
block|,
literal|0xb59b4d10
block|,
literal|0x48719063
block|,
literal|0xe32824b2
block|,
literal|0x1483517e
block|,
literal|0xbfdae5af
block|,
literal|0x423038dc
block|,
literal|0xe9698c0d
block|,
literal|0x12bc36eb
block|,
literal|0xb9e5823a
block|,
literal|0x440f5f49
block|,
literal|0xef56eb98
block|,
literal|0x31fb3ca8
block|,
literal|0x9aa28879
block|,
literal|0x6748550a
block|,
literal|0xcc11e1db
block|,
literal|0x37c45b3d
block|,
literal|0x9c9defec
block|,
literal|0x6177329f
block|,
literal|0xca2e864e
block|,
literal|0x3d85f382
block|,
literal|0x96dc4753
block|,
literal|0x6b369a20
block|,
literal|0xc06f2ef1
block|,
literal|0x3bba9417
block|,
literal|0x90e320c6
block|,
literal|0x6d09fdb5
block|,
literal|0xc6504964
block|,
literal|0x2906a2fc
block|,
literal|0x825f162d
block|,
literal|0x7fb5cb5e
block|,
literal|0xd4ec7f8f
block|,
literal|0x2f39c569
block|,
literal|0x846071b8
block|,
literal|0x798aaccb
block|,
literal|0xd2d3181a
block|,
literal|0x25786dd6
block|,
literal|0x8e21d907
block|,
literal|0x73cb0474
block|,
literal|0xd892b0a5
block|,
literal|0x23470a43
block|,
literal|0x881ebe92
block|,
literal|0x75f463e1
block|,
literal|0xdeadd730
block|,
literal|0x63f67950
block|,
literal|0xc8afcd81
block|,
literal|0x354510f2
block|,
literal|0x9e1ca423
block|,
literal|0x65c91ec5
block|,
literal|0xce90aa14
block|,
literal|0x337a7767
block|,
literal|0x9823c3b6
block|,
literal|0x6f88b67a
block|,
literal|0xc4d102ab
block|,
literal|0x393bdfd8
block|,
literal|0x92626b09
block|,
literal|0x69b7d1ef
block|,
literal|0xc2ee653e
block|,
literal|0x3f04b84d
block|,
literal|0x945d0c9c
block|,
literal|0x7b0be704
block|,
literal|0xd05253d5
block|,
literal|0x2db88ea6
block|,
literal|0x86e13a77
block|,
literal|0x7d348091
block|,
literal|0xd66d3440
block|,
literal|0x2b87e933
block|,
literal|0x80de5de2
block|,
literal|0x7775282e
block|,
literal|0xdc2c9cff
block|,
literal|0x21c6418c
block|,
literal|0x8a9ff55d
block|,
literal|0x714a4fbb
block|,
literal|0xda13fb6a
block|,
literal|0x27f92619
block|,
literal|0x8ca092c8
block|,
literal|0x520d45f8
block|,
literal|0xf954f129
block|,
literal|0x04be2c5a
block|,
literal|0xafe7988b
block|,
literal|0x5432226d
block|,
literal|0xff6b96bc
block|,
literal|0x02814bcf
block|,
literal|0xa9d8ff1e
block|,
literal|0x5e738ad2
block|,
literal|0xf52a3e03
block|,
literal|0x08c0e370
block|,
literal|0xa39957a1
block|,
literal|0x584ced47
block|,
literal|0xf3155996
block|,
literal|0x0eff84e5
block|,
literal|0xa5a63034
block|,
literal|0x4af0dbac
block|,
literal|0xe1a96f7d
block|,
literal|0x1c43b20e
block|,
literal|0xb71a06df
block|,
literal|0x4ccfbc39
block|,
literal|0xe79608e8
block|,
literal|0x1a7cd59b
block|,
literal|0xb125614a
block|,
literal|0x468e1486
block|,
literal|0xedd7a057
block|,
literal|0x103d7d24
block|,
literal|0xbb64c9f5
block|,
literal|0x40b17313
block|,
literal|0xebe8c7c2
block|,
literal|0x16021ab1
block|,
literal|0xbd5bae60
block|,
literal|0x6cb54671
block|,
literal|0xc7ecf2a0
block|,
literal|0x3a062fd3
block|,
literal|0x915f9b02
block|,
literal|0x6a8a21e4
block|,
literal|0xc1d39535
block|,
literal|0x3c394846
block|,
literal|0x9760fc97
block|,
literal|0x60cb895b
block|,
literal|0xcb923d8a
block|,
literal|0x3678e0f9
block|,
literal|0x9d215428
block|,
literal|0x66f4eece
block|,
literal|0xcdad5a1f
block|,
literal|0x3047876c
block|,
literal|0x9b1e33bd
block|,
literal|0x7448d825
block|,
literal|0xdf116cf4
block|,
literal|0x22fbb187
block|,
literal|0x89a20556
block|,
literal|0x7277bfb0
block|,
literal|0xd92e0b61
block|,
literal|0x24c4d612
block|,
literal|0x8f9d62c3
block|,
literal|0x7836170f
block|,
literal|0xd36fa3de
block|,
literal|0x2e857ead
block|,
literal|0x85dcca7c
block|,
literal|0x7e09709a
block|,
literal|0xd550c44b
block|,
literal|0x28ba1938
block|,
literal|0x83e3ade9
block|,
literal|0x5d4e7ad9
block|,
literal|0xf617ce08
block|,
literal|0x0bfd137b
block|,
literal|0xa0a4a7aa
block|,
literal|0x5b711d4c
block|,
literal|0xf028a99d
block|,
literal|0x0dc274ee
block|,
literal|0xa69bc03f
block|,
literal|0x5130b5f3
block|,
literal|0xfa690122
block|,
literal|0x0783dc51
block|,
literal|0xacda6880
block|,
literal|0x570fd266
block|,
literal|0xfc5666b7
block|,
literal|0x01bcbbc4
block|,
literal|0xaae50f15
block|,
literal|0x45b3e48d
block|,
literal|0xeeea505c
block|,
literal|0x13008d2f
block|,
literal|0xb85939fe
block|,
literal|0x438c8318
block|,
literal|0xe8d537c9
block|,
literal|0x153feaba
block|,
literal|0xbe665e6b
block|,
literal|0x49cd2ba7
block|,
literal|0xe2949f76
block|,
literal|0x1f7e4205
block|,
literal|0xb427f6d4
block|,
literal|0x4ff24c32
block|,
literal|0xe4abf8e3
block|,
literal|0x19412590
block|,
literal|0xb2189141
block|,
literal|0x0f433f21
block|,
literal|0xa41a8bf0
block|,
literal|0x59f05683
block|,
literal|0xf2a9e252
block|,
literal|0x097c58b4
block|,
literal|0xa225ec65
block|,
literal|0x5fcf3116
block|,
literal|0xf49685c7
block|,
literal|0x033df00b
block|,
literal|0xa86444da
block|,
literal|0x558e99a9
block|,
literal|0xfed72d78
block|,
literal|0x0502979e
block|,
literal|0xae5b234f
block|,
literal|0x53b1fe3c
block|,
literal|0xf8e84aed
block|,
literal|0x17bea175
block|,
literal|0xbce715a4
block|,
literal|0x410dc8d7
block|,
literal|0xea547c06
block|,
literal|0x1181c6e0
block|,
literal|0xbad87231
block|,
literal|0x4732af42
block|,
literal|0xec6b1b93
block|,
literal|0x1bc06e5f
block|,
literal|0xb099da8e
block|,
literal|0x4d7307fd
block|,
literal|0xe62ab32c
block|,
literal|0x1dff09ca
block|,
literal|0xb6a6bd1b
block|,
literal|0x4b4c6068
block|,
literal|0xe015d4b9
block|,
literal|0x3eb80389
block|,
literal|0x95e1b758
block|,
literal|0x680b6a2b
block|,
literal|0xc352defa
block|,
literal|0x3887641c
block|,
literal|0x93ded0cd
block|,
literal|0x6e340dbe
block|,
literal|0xc56db96f
block|,
literal|0x32c6cca3
block|,
literal|0x999f7872
block|,
literal|0x6475a501
block|,
literal|0xcf2c11d0
block|,
literal|0x34f9ab36
block|,
literal|0x9fa01fe7
block|,
literal|0x624ac294
block|,
literal|0xc9137645
block|,
literal|0x26459ddd
block|,
literal|0x8d1c290c
block|,
literal|0x70f6f47f
block|,
literal|0xdbaf40ae
block|,
literal|0x207afa48
block|,
literal|0x8b234e99
block|,
literal|0x76c993ea
block|,
literal|0xdd90273b
block|,
literal|0x2a3b52f7
block|,
literal|0x8162e626
block|,
literal|0x7c883b55
block|,
literal|0xd7d18f84
block|,
literal|0x2c043562
block|,
literal|0x875d81b3
block|,
literal|0x7ab75cc0
block|,
literal|0xd1eee811
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|U
specifier|static
specifier|const
name|unsigned
name|int
name|U
index|[
literal|256
index|]
init|=
block|{
literal|0x00000000
block|,
literal|0x7eb5200d
block|,
literal|0x5633f4cb
block|,
literal|0x2886d4c6
block|,
literal|0x073e5d47
block|,
literal|0x798b7d4a
block|,
literal|0x510da98c
block|,
literal|0x2fb88981
block|,
literal|0x0e7cba8e
block|,
literal|0x70c99a83
block|,
literal|0x584f4e45
block|,
literal|0x26fa6e48
block|,
literal|0x0942e7c9
block|,
literal|0x77f7c7c4
block|,
literal|0x5f711302
block|,
literal|0x21c4330f
block|,
literal|0x1cf9751c
block|,
literal|0x624c5511
block|,
literal|0x4aca81d7
block|,
literal|0x347fa1da
block|,
literal|0x1bc7285b
block|,
literal|0x65720856
block|,
literal|0x4df4dc90
block|,
literal|0x3341fc9d
block|,
literal|0x1285cf92
block|,
literal|0x6c30ef9f
block|,
literal|0x44b63b59
block|,
literal|0x3a031b54
block|,
literal|0x15bb92d5
block|,
literal|0x6b0eb2d8
block|,
literal|0x4388661e
block|,
literal|0x3d3d4613
block|,
literal|0x39f2ea38
block|,
literal|0x4747ca35
block|,
literal|0x6fc11ef3
block|,
literal|0x11743efe
block|,
literal|0x3eccb77f
block|,
literal|0x40799772
block|,
literal|0x68ff43b4
block|,
literal|0x164a63b9
block|,
literal|0x378e50b6
block|,
literal|0x493b70bb
block|,
literal|0x61bda47d
block|,
literal|0x1f088470
block|,
literal|0x30b00df1
block|,
literal|0x4e052dfc
block|,
literal|0x6683f93a
block|,
literal|0x1836d937
block|,
literal|0x250b9f24
block|,
literal|0x5bbebf29
block|,
literal|0x73386bef
block|,
literal|0x0d8d4be2
block|,
literal|0x2235c263
block|,
literal|0x5c80e26e
block|,
literal|0x740636a8
block|,
literal|0x0ab316a5
block|,
literal|0x2b7725aa
block|,
literal|0x55c205a7
block|,
literal|0x7d44d161
block|,
literal|0x03f1f16c
block|,
literal|0x2c4978ed
block|,
literal|0x52fc58e0
block|,
literal|0x7a7a8c26
block|,
literal|0x04cfac2b
block|,
literal|0x73e5d470
block|,
literal|0x0d50f47d
block|,
literal|0x25d620bb
block|,
literal|0x5b6300b6
block|,
literal|0x74db8937
block|,
literal|0x0a6ea93a
block|,
literal|0x22e87dfc
block|,
literal|0x5c5d5df1
block|,
literal|0x7d996efe
block|,
literal|0x032c4ef3
block|,
literal|0x2baa9a35
block|,
literal|0x551fba38
block|,
literal|0x7aa733b9
block|,
literal|0x041213b4
block|,
literal|0x2c94c772
block|,
literal|0x5221e77f
block|,
literal|0x6f1ca16c
block|,
literal|0x11a98161
block|,
literal|0x392f55a7
block|,
literal|0x479a75aa
block|,
literal|0x6822fc2b
block|,
literal|0x1697dc26
block|,
literal|0x3e1108e0
block|,
literal|0x40a428ed
block|,
literal|0x61601be2
block|,
literal|0x1fd53bef
block|,
literal|0x3753ef29
block|,
literal|0x49e6cf24
block|,
literal|0x665e46a5
block|,
literal|0x18eb66a8
block|,
literal|0x306db26e
block|,
literal|0x4ed89263
block|,
literal|0x4a173e48
block|,
literal|0x34a21e45
block|,
literal|0x1c24ca83
block|,
literal|0x6291ea8e
block|,
literal|0x4d29630f
block|,
literal|0x339c4302
block|,
literal|0x1b1a97c4
block|,
literal|0x65afb7c9
block|,
literal|0x446b84c6
block|,
literal|0x3adea4cb
block|,
literal|0x1258700d
block|,
literal|0x6ced5000
block|,
literal|0x4355d981
block|,
literal|0x3de0f98c
block|,
literal|0x15662d4a
block|,
literal|0x6bd30d47
block|,
literal|0x56ee4b54
block|,
literal|0x285b6b59
block|,
literal|0x00ddbf9f
block|,
literal|0x7e689f92
block|,
literal|0x51d01613
block|,
literal|0x2f65361e
block|,
literal|0x07e3e2d8
block|,
literal|0x7956c2d5
block|,
literal|0x5892f1da
block|,
literal|0x2627d1d7
block|,
literal|0x0ea10511
block|,
literal|0x7014251c
block|,
literal|0x5facac9d
block|,
literal|0x21198c90
block|,
literal|0x099f5856
block|,
literal|0x772a785b
block|,
literal|0x4c921c31
block|,
literal|0x32273c3c
block|,
literal|0x1aa1e8fa
block|,
literal|0x6414c8f7
block|,
literal|0x4bac4176
block|,
literal|0x3519617b
block|,
literal|0x1d9fb5bd
block|,
literal|0x632a95b0
block|,
literal|0x42eea6bf
block|,
literal|0x3c5b86b2
block|,
literal|0x14dd5274
block|,
literal|0x6a687279
block|,
literal|0x45d0fbf8
block|,
literal|0x3b65dbf5
block|,
literal|0x13e30f33
block|,
literal|0x6d562f3e
block|,
literal|0x506b692d
block|,
literal|0x2ede4920
block|,
literal|0x06589de6
block|,
literal|0x78edbdeb
block|,
literal|0x5755346a
block|,
literal|0x29e01467
block|,
literal|0x0166c0a1
block|,
literal|0x7fd3e0ac
block|,
literal|0x5e17d3a3
block|,
literal|0x20a2f3ae
block|,
literal|0x08242768
block|,
literal|0x76910765
block|,
literal|0x59298ee4
block|,
literal|0x279caee9
block|,
literal|0x0f1a7a2f
block|,
literal|0x71af5a22
block|,
literal|0x7560f609
block|,
literal|0x0bd5d604
block|,
literal|0x235302c2
block|,
literal|0x5de622cf
block|,
literal|0x725eab4e
block|,
literal|0x0ceb8b43
block|,
literal|0x246d5f85
block|,
literal|0x5ad87f88
block|,
literal|0x7b1c4c87
block|,
literal|0x05a96c8a
block|,
literal|0x2d2fb84c
block|,
literal|0x539a9841
block|,
literal|0x7c2211c0
block|,
literal|0x029731cd
block|,
literal|0x2a11e50b
block|,
literal|0x54a4c506
block|,
literal|0x69998315
block|,
literal|0x172ca318
block|,
literal|0x3faa77de
block|,
literal|0x411f57d3
block|,
literal|0x6ea7de52
block|,
literal|0x1012fe5f
block|,
literal|0x38942a99
block|,
literal|0x46210a94
block|,
literal|0x67e5399b
block|,
literal|0x19501996
block|,
literal|0x31d6cd50
block|,
literal|0x4f63ed5d
block|,
literal|0x60db64dc
block|,
literal|0x1e6e44d1
block|,
literal|0x36e89017
block|,
literal|0x485db01a
block|,
literal|0x3f77c841
block|,
literal|0x41c2e84c
block|,
literal|0x69443c8a
block|,
literal|0x17f11c87
block|,
literal|0x38499506
block|,
literal|0x46fcb50b
block|,
literal|0x6e7a61cd
block|,
literal|0x10cf41c0
block|,
literal|0x310b72cf
block|,
literal|0x4fbe52c2
block|,
literal|0x67388604
block|,
literal|0x198da609
block|,
literal|0x36352f88
block|,
literal|0x48800f85
block|,
literal|0x6006db43
block|,
literal|0x1eb3fb4e
block|,
literal|0x238ebd5d
block|,
literal|0x5d3b9d50
block|,
literal|0x75bd4996
block|,
literal|0x0b08699b
block|,
literal|0x24b0e01a
block|,
literal|0x5a05c017
block|,
literal|0x728314d1
block|,
literal|0x0c3634dc
block|,
literal|0x2df207d3
block|,
literal|0x534727de
block|,
literal|0x7bc1f318
block|,
literal|0x0574d315
block|,
literal|0x2acc5a94
block|,
literal|0x54797a99
block|,
literal|0x7cffae5f
block|,
literal|0x024a8e52
block|,
literal|0x06852279
block|,
literal|0x78300274
block|,
literal|0x50b6d6b2
block|,
literal|0x2e03f6bf
block|,
literal|0x01bb7f3e
block|,
literal|0x7f0e5f33
block|,
literal|0x57888bf5
block|,
literal|0x293dabf8
block|,
literal|0x08f998f7
block|,
literal|0x764cb8fa
block|,
literal|0x5eca6c3c
block|,
literal|0x207f4c31
block|,
literal|0x0fc7c5b0
block|,
literal|0x7172e5bd
block|,
literal|0x59f4317b
block|,
literal|0x27411176
block|,
literal|0x1a7c5765
block|,
literal|0x64c97768
block|,
literal|0x4c4fa3ae
block|,
literal|0x32fa83a3
block|,
literal|0x1d420a22
block|,
literal|0x63f72a2f
block|,
literal|0x4b71fee9
block|,
literal|0x35c4dee4
block|,
literal|0x1400edeb
block|,
literal|0x6ab5cde6
block|,
literal|0x42331920
block|,
literal|0x3c86392d
block|,
literal|0x133eb0ac
block|,
literal|0x6d8b90a1
block|,
literal|0x450d4467
block|,
literal|0x3bb8646a
block|}
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|index_entry
struct|struct
name|index_entry
block|{
DECL|member|ptr
specifier|const
name|unsigned
name|char
modifier|*
name|ptr
decl_stmt|;
DECL|member|val
name|unsigned
name|int
name|val
decl_stmt|;
DECL|member|next
name|struct
name|index_entry
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|delta_index
struct|struct
name|delta_index
block|{
DECL|member|memsize
name|unsigned
name|long
name|memsize
decl_stmt|;
DECL|member|src_buf
specifier|const
name|void
modifier|*
name|src_buf
decl_stmt|;
DECL|member|src_size
name|unsigned
name|long
name|src_size
decl_stmt|;
DECL|member|hash_mask
name|unsigned
name|int
name|hash_mask
decl_stmt|;
DECL|member|hash
name|struct
name|index_entry
modifier|*
name|hash
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|create_delta_index
name|struct
name|delta_index
modifier|*
name|create_delta_index
parameter_list|(
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|bufsize
parameter_list|)
block|{
name|unsigned
name|int
name|i
decl_stmt|,
name|hsize
decl_stmt|,
name|hmask
decl_stmt|,
name|entries
decl_stmt|,
name|prev_val
decl_stmt|,
modifier|*
name|hash_count
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|buffer
init|=
name|buf
decl_stmt|;
name|struct
name|delta_index
modifier|*
name|index
decl_stmt|;
name|struct
name|index_entry
modifier|*
name|entry
decl_stmt|,
modifier|*
modifier|*
name|hash
decl_stmt|;
name|void
modifier|*
name|mem
decl_stmt|;
name|unsigned
name|long
name|memsize
decl_stmt|;
if|if
condition|(
operator|!
name|buf
operator|||
operator|!
name|bufsize
condition|)
return|return
name|NULL
return|;
comment|/* Determine index hash size.  Note that indexing skips the 	   first byte to allow for optimizing the Rabin's polynomial 	   initialization in create_delta(). */
name|entries
operator|=
operator|(
name|bufsize
operator|-
literal|1
operator|)
operator|/
name|RABIN_WINDOW
expr_stmt|;
name|hsize
operator|=
name|entries
operator|/
literal|4
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|4
init|;
operator|(
literal|1u
operator|<<
name|i
operator|)
operator|<
name|hsize
operator|&&
name|i
operator|<
literal|31
condition|;
name|i
operator|++
control|)
empty_stmt|;
name|hsize
operator|=
literal|1
operator|<<
name|i
expr_stmt|;
name|hmask
operator|=
name|hsize
operator|-
literal|1
expr_stmt|;
comment|/* allocate lookup index */
name|memsize
operator|=
sizeof|sizeof
argument_list|(
operator|*
name|index
argument_list|)
operator|+
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
operator|*
name|hsize
operator|+
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
operator|*
name|entries
expr_stmt|;
name|mem
operator|=
name|malloc
argument_list|(
name|memsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mem
condition|)
return|return
name|NULL
return|;
name|index
operator|=
name|mem
expr_stmt|;
name|mem
operator|=
name|index
operator|+
literal|1
expr_stmt|;
name|hash
operator|=
name|mem
expr_stmt|;
name|mem
operator|=
name|hash
operator|+
name|hsize
expr_stmt|;
name|entry
operator|=
name|mem
expr_stmt|;
name|index
operator|->
name|memsize
operator|=
name|memsize
expr_stmt|;
name|index
operator|->
name|src_buf
operator|=
name|buf
expr_stmt|;
name|index
operator|->
name|src_size
operator|=
name|bufsize
expr_stmt|;
name|index
operator|->
name|hash_mask
operator|=
name|hmask
expr_stmt|;
name|memset
argument_list|(
name|hash
argument_list|,
literal|0
argument_list|,
name|hsize
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
argument_list|)
expr_stmt|;
comment|/* allocate an array to count hash entries */
name|hash_count
operator|=
name|calloc
argument_list|(
name|hsize
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|hash_count
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|hash_count
condition|)
block|{
name|free
argument_list|(
name|index
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* then populate the index */
name|prev_val
operator|=
operator|~
literal|0
expr_stmt|;
for|for
control|(
name|data
operator|=
name|buffer
operator|+
name|entries
operator|*
name|RABIN_WINDOW
operator|-
name|RABIN_WINDOW
init|;
name|data
operator|>=
name|buffer
condition|;
name|data
operator|-=
name|RABIN_WINDOW
control|)
block|{
name|unsigned
name|int
name|val
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<=
name|RABIN_WINDOW
condition|;
name|i
operator|++
control|)
name|val
operator|=
operator|(
operator|(
name|val
operator|<<
literal|8
operator|)
operator||
name|data
index|[
name|i
index|]
operator|)
operator|^
name|T
index|[
name|val
operator|>>
name|RABIN_SHIFT
index|]
expr_stmt|;
if|if
condition|(
name|val
operator|==
name|prev_val
condition|)
block|{
comment|/* keep the lowest of consecutive identical blocks */
name|entry
index|[
operator|-
literal|1
index|]
operator|.
name|ptr
operator|=
name|data
operator|+
name|RABIN_WINDOW
expr_stmt|;
block|}
else|else
block|{
name|prev_val
operator|=
name|val
expr_stmt|;
name|i
operator|=
name|val
operator|&
name|hmask
expr_stmt|;
name|entry
operator|->
name|ptr
operator|=
name|data
operator|+
name|RABIN_WINDOW
expr_stmt|;
name|entry
operator|->
name|val
operator|=
name|val
expr_stmt|;
name|entry
operator|->
name|next
operator|=
name|hash
index|[
name|i
index|]
expr_stmt|;
name|hash
index|[
name|i
index|]
operator|=
name|entry
operator|++
expr_stmt|;
name|hash_count
index|[
name|i
index|]
operator|++
expr_stmt|;
block|}
block|}
comment|/* 	 * Determine a limit on the number of entries in the same hash 	 * bucket.  This guards us against pathological data sets causing 	 * really bad hash distribution with most entries in the same hash 	 * bucket that would bring us to O(m*n) computing costs (m and n 	 * corresponding to reference and target buffer sizes). 	 * 	 * Make sure none of the hash buckets has more entries than 	 * we're willing to test.  Otherwise we cull the entry list 	 * uniformly to still preserve a good repartition across 	 * the reference buffer. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|hsize
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|hash_count
index|[
name|i
index|]
operator|<
name|HASH_LIMIT
condition|)
continue|continue;
name|entry
operator|=
name|hash
index|[
name|i
index|]
expr_stmt|;
do|do
block|{
name|struct
name|index_entry
modifier|*
name|keep
init|=
name|entry
decl_stmt|;
name|int
name|skip
init|=
name|hash_count
index|[
name|i
index|]
operator|/
name|HASH_LIMIT
operator|/
literal|2
decl_stmt|;
do|do
block|{
name|entry
operator|=
name|entry
operator|->
name|next
expr_stmt|;
block|}
do|while
condition|(
operator|--
name|skip
operator|&&
name|entry
condition|)
do|;
name|keep
operator|->
name|next
operator|=
name|entry
expr_stmt|;
block|}
do|while
condition|(
name|entry
condition|)
do|;
block|}
name|free
argument_list|(
name|hash_count
argument_list|)
expr_stmt|;
return|return
name|index
return|;
block|}
end_function
begin_function
DECL|function|free_delta_index
name|void
name|free_delta_index
parameter_list|(
name|struct
name|delta_index
modifier|*
name|index
parameter_list|)
block|{
name|free
argument_list|(
name|index
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sizeof_delta_index
name|unsigned
name|long
name|sizeof_delta_index
parameter_list|(
name|struct
name|delta_index
modifier|*
name|index
parameter_list|)
block|{
if|if
condition|(
name|index
condition|)
return|return
name|index
operator|->
name|memsize
return|;
else|else
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * The maximum size for any opcode sequence, including the initial header  * plus Rabin window plus biggest copy.  */
end_comment
begin_define
DECL|macro|MAX_OP_SIZE
define|#
directive|define
name|MAX_OP_SIZE
value|(5 + 5 + 1 + RABIN_WINDOW + 7)
end_define
begin_function
name|void
modifier|*
DECL|function|create_delta
name|create_delta
parameter_list|(
specifier|const
name|struct
name|delta_index
modifier|*
name|index
parameter_list|,
specifier|const
name|void
modifier|*
name|trg_buf
parameter_list|,
name|unsigned
name|long
name|trg_size
parameter_list|,
name|unsigned
name|long
modifier|*
name|delta_size
parameter_list|,
name|unsigned
name|long
name|max_size
parameter_list|)
block|{
name|unsigned
name|int
name|i
decl_stmt|,
name|outpos
decl_stmt|,
name|outsize
decl_stmt|,
name|moff
decl_stmt|,
name|msize
decl_stmt|,
name|val
decl_stmt|;
name|int
name|inscnt
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|ref_data
decl_stmt|,
modifier|*
name|ref_top
decl_stmt|,
modifier|*
name|data
decl_stmt|,
modifier|*
name|top
decl_stmt|;
name|unsigned
name|char
modifier|*
name|out
decl_stmt|;
if|if
condition|(
operator|!
name|trg_buf
operator|||
operator|!
name|trg_size
condition|)
return|return
name|NULL
return|;
name|outpos
operator|=
literal|0
expr_stmt|;
name|outsize
operator|=
literal|8192
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outsize
operator|>=
name|max_size
condition|)
name|outsize
operator|=
name|max_size
operator|+
name|MAX_OP_SIZE
operator|+
literal|1
expr_stmt|;
name|out
operator|=
name|malloc
argument_list|(
name|outsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out
condition|)
return|return
name|NULL
return|;
comment|/* store reference buffer size */
name|i
operator|=
name|index
operator|->
name|src_size
expr_stmt|;
while|while
condition|(
name|i
operator|>=
literal|0x80
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|i
operator||
literal|0x80
expr_stmt|;
name|i
operator|>>=
literal|7
expr_stmt|;
block|}
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|i
expr_stmt|;
comment|/* store target buffer size */
name|i
operator|=
name|trg_size
expr_stmt|;
while|while
condition|(
name|i
operator|>=
literal|0x80
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|i
operator||
literal|0x80
expr_stmt|;
name|i
operator|>>=
literal|7
expr_stmt|;
block|}
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|i
expr_stmt|;
name|ref_data
operator|=
name|index
operator|->
name|src_buf
expr_stmt|;
name|ref_top
operator|=
name|ref_data
operator|+
name|index
operator|->
name|src_size
expr_stmt|;
name|data
operator|=
name|trg_buf
expr_stmt|;
name|top
operator|=
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|trg_buf
operator|+
name|trg_size
expr_stmt|;
name|outpos
operator|++
expr_stmt|;
name|val
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|RABIN_WINDOW
operator|&&
name|data
operator|<
name|top
condition|;
name|i
operator|++
operator|,
name|data
operator|++
control|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
operator|*
name|data
expr_stmt|;
name|val
operator|=
operator|(
operator|(
name|val
operator|<<
literal|8
operator|)
operator||
operator|*
name|data
operator|)
operator|^
name|T
index|[
name|val
operator|>>
name|RABIN_SHIFT
index|]
expr_stmt|;
block|}
name|inscnt
operator|=
name|i
expr_stmt|;
name|moff
operator|=
literal|0
expr_stmt|;
name|msize
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|data
operator|<
name|top
condition|)
block|{
if|if
condition|(
name|msize
operator|<
literal|4096
condition|)
block|{
name|struct
name|index_entry
modifier|*
name|entry
decl_stmt|;
name|val
operator|^=
name|U
index|[
name|data
index|[
operator|-
name|RABIN_WINDOW
index|]
index|]
expr_stmt|;
name|val
operator|=
operator|(
operator|(
name|val
operator|<<
literal|8
operator|)
operator||
operator|*
name|data
operator|)
operator|^
name|T
index|[
name|val
operator|>>
name|RABIN_SHIFT
index|]
expr_stmt|;
name|i
operator|=
name|val
operator|&
name|index
operator|->
name|hash_mask
expr_stmt|;
for|for
control|(
name|entry
operator|=
name|index
operator|->
name|hash
index|[
name|i
index|]
init|;
name|entry
condition|;
name|entry
operator|=
name|entry
operator|->
name|next
control|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|ref
init|=
name|entry
operator|->
name|ptr
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|src
init|=
name|data
decl_stmt|;
name|unsigned
name|int
name|ref_size
init|=
name|ref_top
operator|-
name|ref
decl_stmt|;
if|if
condition|(
name|entry
operator|->
name|val
operator|!=
name|val
condition|)
continue|continue;
if|if
condition|(
name|ref_size
operator|>
name|top
operator|-
name|src
condition|)
name|ref_size
operator|=
name|top
operator|-
name|src
expr_stmt|;
if|if
condition|(
name|ref_size
operator|<=
name|msize
condition|)
break|break;
while|while
condition|(
name|ref_size
operator|--
operator|&&
operator|*
name|src
operator|++
operator|==
operator|*
name|ref
condition|)
name|ref
operator|++
expr_stmt|;
if|if
condition|(
name|msize
operator|<
name|ref
operator|-
name|entry
operator|->
name|ptr
condition|)
block|{
comment|/* this is our best match so far */
name|msize
operator|=
name|ref
operator|-
name|entry
operator|->
name|ptr
expr_stmt|;
name|moff
operator|=
name|entry
operator|->
name|ptr
operator|-
name|ref_data
expr_stmt|;
if|if
condition|(
name|msize
operator|>=
literal|4096
condition|)
comment|/* good enough */
break|break;
block|}
block|}
block|}
if|if
condition|(
name|msize
operator|<
literal|4
condition|)
block|{
if|if
condition|(
operator|!
name|inscnt
condition|)
name|outpos
operator|++
expr_stmt|;
name|out
index|[
name|outpos
operator|++
index|]
operator|=
operator|*
name|data
operator|++
expr_stmt|;
name|inscnt
operator|++
expr_stmt|;
if|if
condition|(
name|inscnt
operator|==
literal|0x7f
condition|)
block|{
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
name|inscnt
operator|=
literal|0
expr_stmt|;
block|}
name|msize
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|unsigned
name|int
name|left
decl_stmt|;
name|unsigned
name|char
modifier|*
name|op
decl_stmt|;
if|if
condition|(
name|inscnt
condition|)
block|{
while|while
condition|(
name|moff
operator|&&
name|ref_data
index|[
name|moff
operator|-
literal|1
index|]
operator|==
name|data
index|[
operator|-
literal|1
index|]
condition|)
block|{
comment|/* we can match one byte back */
name|msize
operator|++
expr_stmt|;
name|moff
operator|--
expr_stmt|;
name|data
operator|--
expr_stmt|;
name|outpos
operator|--
expr_stmt|;
if|if
condition|(
operator|--
name|inscnt
condition|)
continue|continue;
name|outpos
operator|--
expr_stmt|;
comment|/* remove count slot */
name|inscnt
operator|--
expr_stmt|;
comment|/* make it -1 */
break|break;
block|}
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
name|inscnt
operator|=
literal|0
expr_stmt|;
block|}
comment|/* A copy op is currently limited to 64KB (pack v2) */
name|left
operator|=
operator|(
name|msize
operator|<
literal|0x10000
operator|)
condition|?
literal|0
else|:
operator|(
name|msize
operator|-
literal|0x10000
operator|)
expr_stmt|;
name|msize
operator|-=
name|left
expr_stmt|;
name|op
operator|=
name|out
operator|+
name|outpos
operator|++
expr_stmt|;
name|i
operator|=
literal|0x80
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0x000000ff
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
operator|>>
literal|0
operator|,
name|i
operator||=
literal|0x01
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0x0000ff00
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
operator|>>
literal|8
operator|,
name|i
operator||=
literal|0x02
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0x00ff0000
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
operator|>>
literal|16
operator|,
name|i
operator||=
literal|0x04
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0xff000000
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
operator|>>
literal|24
operator|,
name|i
operator||=
literal|0x08
expr_stmt|;
if|if
condition|(
name|msize
operator|&
literal|0x00ff
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|msize
operator|>>
literal|0
operator|,
name|i
operator||=
literal|0x10
expr_stmt|;
if|if
condition|(
name|msize
operator|&
literal|0xff00
condition|)
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|msize
operator|>>
literal|8
operator|,
name|i
operator||=
literal|0x20
expr_stmt|;
operator|*
name|op
operator|=
name|i
expr_stmt|;
name|data
operator|+=
name|msize
expr_stmt|;
name|moff
operator|+=
name|msize
expr_stmt|;
name|msize
operator|=
name|left
expr_stmt|;
if|if
condition|(
name|msize
operator|<
literal|4096
condition|)
block|{
name|int
name|j
decl_stmt|;
name|val
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|j
operator|=
operator|-
name|RABIN_WINDOW
init|;
name|j
operator|<
literal|0
condition|;
name|j
operator|++
control|)
name|val
operator|=
operator|(
operator|(
name|val
operator|<<
literal|8
operator|)
operator||
name|data
index|[
name|j
index|]
operator|)
operator|^
name|T
index|[
name|val
operator|>>
name|RABIN_SHIFT
index|]
expr_stmt|;
block|}
block|}
if|if
condition|(
name|outpos
operator|>=
name|outsize
operator|-
name|MAX_OP_SIZE
condition|)
block|{
name|void
modifier|*
name|tmp
init|=
name|out
decl_stmt|;
name|outsize
operator|=
name|outsize
operator|*
literal|3
operator|/
literal|2
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outsize
operator|>=
name|max_size
condition|)
name|outsize
operator|=
name|max_size
operator|+
name|MAX_OP_SIZE
operator|+
literal|1
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outpos
operator|>
name|max_size
condition|)
break|break;
name|out
operator|=
name|realloc
argument_list|(
name|out
argument_list|,
name|outsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out
condition|)
block|{
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
block|}
if|if
condition|(
name|inscnt
condition|)
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outpos
operator|>
name|max_size
condition|)
block|{
name|free
argument_list|(
name|out
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
operator|*
name|delta_size
operator|=
name|outpos
expr_stmt|;
return|return
name|out
return|;
block|}
end_function
end_unit
