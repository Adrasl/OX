
//Permissions
#define PERM_PSI_VOID			0x000000	//000000000		//Tabula Rasa
#define PERM_PSI_MODIFY			0x000007	//000000111		//Psique can be modified by:	
#define PERM_PSI_ITSELF_MODIFY	0x000001	//000000010		//by itself
#define PERM_PSI_OWNER_MODIFY	0x000002	//000000100		//by owner actions
#define PERM_OWNER_SEE			0x000004	//000001000		//Owner can see
#define PERM_OWNER_LISTEN		0x000008	//000001000		//Owner can see
#define PERM_OWNER_TOUCH		0x000010	//000010000		//Owner can touch
#define PERM_OWNER_PICK			0x000020	//000110000		//Owner can pick thinks
#define PERM_OWNER_CREATE		0x000040	//001000000		//Owner can create
#define PERM_OWNER_DESTROY	    0x000080	//010000000		//Owner can destroy
#define PERM_OWNER_ALL			0x0000FF	//011111111		//Owner can do anything
#define PERM_SOLO				0x000080	//100000000		//Solo mode
#define PERM_FREE_WILL			0x0000FF	//111111111		//Anybody can do anything
//Ready for future work									//Psique can be modified by:
#define PERM_PSI_OTHERS_MODIFY	0x000400	//0000000		//by others
#define PERM_OTHERS_SEE			0x000800	//0000000		//Others can see
#define PERM_OTHERS_TOUCH		0x000F00	//0000000		//Others can touch
#define PERM_OTHERS_PICK		0x001F00	//0000000		//Others can pick thinks
#define PERM_OTHERS_CREATE		0x002000	//0000000		//Others can create
#define PERM_OTHERS_DESTROY		0x004000	//0000000		//Others can destroy
#define PERM_OTHERS_ALL			0x007F00	//0000000		//Others can do anything