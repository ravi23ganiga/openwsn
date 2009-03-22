
typedef struct{
}TiObjectStore;

obst_construct
obst_destroy
obst_evolve
  call the evolvehandler(owner) of each objects in the list
obst_put( obst, evolvehandler, owner )
obst_get()
obst_itemcount
obst_items()

/* objectstore is one of the fundmental kernel component in OpenWSN architecture.
 * it mains a list of objects and capable evolve them. 
 * 
 * theorectically, the objectstore can manage the whole lifetime of the objects,
 * including their creation and free. however, openwsn is a very lightweight 
 * embedded system, so we want to simplify the resource management and let objectstore
 * maintains the object list and do evolve() only. 
 * 
 * since the openwsn developer will manage the create/free of objects themselves, 
 * the memory can be controlled well.
 *
 * [chinese translation]
 * objectstore��openwsn��������֮һ
 * �����Ͻ���objectstore���Թ������������������ڰ���create/free��������openwsn�У�
 * ������Դ���ƺͼ��������objectstore������������evolve��������create/free
 */

typedef bool (*TiPFunEvolve(void * owner))

typedef struct{
  TiPFunEvolve objlist[CONFIG_OBJSTORE_CAPACITY];
  void * ownerlist[CONFIG_OBJSTORE_CAPACITY];
}TObjectStoreItem;

typedef struct{
  
}TObjectStore;


objectstore.put()
objectstore.get
objectstore.items(idx)
objectstore.evolve


