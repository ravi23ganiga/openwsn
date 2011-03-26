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

#ifndef _SVC_INTERPRETER_H_6489_
#define _SVC_INTERPRETER_H_6489_

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

����֡�ڸ�ʽ����

/* svc_interpreter
 * ����֡����ִ�����
 * ���͵����ݾ���frame�еĹ�service���ʹ�õ�command�ֽڡ�
 * interpreter������յ�frame arrival�¼��󣬻��MAC���ȡ��frame��Ȼ�������д���
 * - �����frame�Ǹ��Լ���(����frame�е�Ŀ���ַ�ж�)�����һ�������´���
 *   o ���Я�����˿�����Ϣ���򽻸�topology control�������
 *   o ���Я��λ����Ϣ���򽻸�localization�������
 *   o ���Я��ͬ�����ݣ��򽻸�time synchronization�������
 *   o ���Я��������ʶ�������򽻸���Ӧģ�鴦��
 * - �����frame���Ǹ��Լ��ģ����route table���ҵ���һ�����ת����ȥ���߹㲥
 *   ��ȥ��
 */

/* Q: interpreter�������õ�MAC�㷢����frame arrival�¼���
 * R: ���ַ�ʽ
 * - interpreter����MAC��setnotifierע��һ����������MAC���յ����ݰ������ø�
 *   listener��listener����һ��������TiFunEventHandler���͵ĺ���ָ�롣interpreter
 *   ��״̬�ݻ������ӿ�evolve()���ɳе��˹��ܣ��������ر���
 *   => ���ǣ������ַ�ʽ�£�evolve()�ܿ��������ж�̬�����еģ���������кܳ�ʱ��
 *   �ܲ���
 * - ����MAC���Դ���ȱʡ��listener�ص���������listener�Ὣframe arrival�¼�push
 *   ��osx kernel��ϵͳ�¼������У�����osx kernel���ս����¼�event����interpreter
 *   ����ִ�С����ַ�ʽͨ��kernel��ת��1�Σ��ɽ���ʱ��interpreter evolve()ִ��
 *   �õ��ж�̬��ִ�У���ʵ�������Ȳ��ã������׳����⡣
 *
 *�� attention: ע��listener�Ĺ�����������е�������interpreter���Բ�ȥ�ܵײ㾿��
 *   ������һ��MAC�����������֮���ճ����������е���
 *
 * Q: interpreter����ڲ��Ƿ��ṩframe����?
 * R: wireless sensor node��Դ���ޣ�����interpreter���ṩ���ݰ����壬�ڲ�ֻ����
 *   һ���������frame�����Ҵ�����ͬ���ģ���ζ�Ŵ���ָ�뼴�ɣ�������frame copy����
 *   ����openwsn�ڲ���ֻ�������ط��ṩframe���壬һ����MAC�ڲ���Ϊ�˱��⵽���
 *   ���ݰ���ʧ��һ����net�����Э���ṩQoS����������Ϊ�˼������ʵ�֡�
 */

/*
interpreter
interpreter_construct
interpreter

intpter_
*/

#ifdef __cplusplus
}
#endif

#endif
