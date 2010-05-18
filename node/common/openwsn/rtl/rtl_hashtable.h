/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
/* Reference
 * ѧϰSTL map, STL set֮���ݽṹ����, 2005, http://stl.winterxy.com/html/000039.html
 * ��ϸ��˵STL hash_mapϵ��, http://www.stlchina.org/twiki/bin/view.pl/Main/STLDetailHashMap
 */

hashtable, map, set��Ҫһ��hash�ӿ�
key = uintx or (buffer pointer, len)

nhash hash table with uintx key
mhash mutibyte hash   (buffer pointer + len, such as string)



hashtable_useinterface( key2index, findempty, find )

hash_key2index_xxxtype
hash_findempty
hash_find



�ҵ���Ҫ��һ��hashtable or  hashmap
uintx id ==> item contents

���Ҫ�󾭳����м����ɾ����listЧ�ʸ���vector
���Ҫ����ٲ��ң�vector������
���Ҫ�����ɾ���Ƚ�ƽ�⣬��heap��tree�ȽϺ�
��������ô˵�������ܹ鶼����Ҫ����һ�����ݽṹ[id, {content}, state, other informaiton such as parent/child]




hash_isempty
hash_findempty
hash_find

[meta information, item state information, item content]
���Կ��ǻ���vectorʵ�֣���vector�����ڴ�





this hash table assume uintx is the key type
{key, value} pair

key = int

vector��������
ֻҪ��������������������
vec_findindex( key )
vec_find(key)
vec_keytoindex(uintx key, uintx idx) = hash function

vec_findemptybykey

vec_mapkey2int

��ԭ�ȵ�vector�޶���ֻ��˳����ӣ���ʵ�Ǹ�list��������ƶ�vector�����ǲ��ǲ�������Ϊvector�������Ǵ���array���ṩһϵ�и�����ķ���
����Ҫ��˳����ʺͲ���

�������Ļ�����β���֪��ĳ��item�Ƿ��Ѿ���ʹ���أ�
û�취

�������ݽṹlist

list
��Ӻ��ͷŶ���O(1)�����������ʵ�֡�����vector�ǲ�ͬ��
����insert��delete��find����
Ϊ�˸Ľ�������TiHashVector

vec_isavailable(item *)
vec_findempty_uintxkey(key)
vec_find_uintx(key)
vec_findempty_charkey(key)
vec_find_uintxkey(key)

vec_hashput
vec_hashget
vec_hashfind
�����vector��Ϊһ��hashtable���ã���ôitem�еĵ�һ���ֽ����ǹ涨����������ʾstate
vec_hashfind( key )
vec_hashfindempty( index )
vec_hashput
vec_hashget
vec_hashitems

�������Ƿ�empty������ô֪��

hash_map/set



