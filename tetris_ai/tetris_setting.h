#ifdef XP_RELEASE
#define PUBLIC_VERSION      1 // 发布模式
#define GAMEMODE_4W         0 // 4w模式
#define PLAYER_WAIT         0 // 玩家等待AI思考，如果需要的话
#define AI_TRAINING_SLOW    1
#define USE4W               1
#else
#define PUBLIC_VERSION      0
#define GAMEMODE_4W         0
#define PLAYER_WAIT         0
#define AI_TRAINING_SLOW    1 // 训练模式慢速演示
#define USE4W               0
#endif
#define ATTACK_MODE         1 // 垃圾行：0空气 1TOP 2火拼
#define AI_SHOW             0 // 不相互攻击，围观AI
#define DISPLAY_NEXT_NUM    6 // 显示next个数
#if AI_TRAINING_SLOW
#define AI_TRAINING_DEEP    16
#else
#define AI_TRAINING_DEEP    6 // 训练AI思考深度
#endif
#define TRAINING_ROUND      20
#define AI_TRAINING_0       9
#define AI_TRAINING_2       6

#define AI_DLL_VERSION      2 // dll版本
