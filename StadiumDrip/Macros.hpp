#ifndef MACROS_H
#define MACROS_H

// convenient macros to avoid repetive typing  (should only be used within main plugin class)

#define DELAY(delaySeconds, code) \
    gameWrapper->SetTimeout([this](GameWrapper* gw) { \
        code \
    }, delaySeconds)


#define DELAY_CAPTURE(delaySeconds, args, code) \
    gameWrapper->SetTimeout([this, args](GameWrapper* gw) { \
        code \
    }, delaySeconds)


#define INTERVAL(delaySeconds, numIntervals, code) \
    for (int i = 0; i < numIntervals; i++) { \
        gameWrapper->SetTimeout([this](GameWrapper* gw) { \
	        code \
	    }, delaySeconds * i) \
    }


#define GAME_THREAD_EXECUTE(code) \
    do { \
        gameWrapper->Execute([this](GameWrapper* gw) { \
            code \
        }); \
    } while (0)


#define GAME_THREAD_EXECUTE_CAPTURE(args, code) \
    do { \
        gameWrapper->Execute([this, args](GameWrapper* gw) { \
            code \
        }); \
    } while (0)


#define DUMMY_THREAD(code) \
    do { \
        std::thread dummy([this]() { \
            code \
        }); \
        dummy.detach(); \
    } while (0)


#define DUMMY_THREAD_CAPTURE(args, code) \
    do { \
        std::thread dummy([this, args]() { \
            code \
        }); \
        dummy.detach(); \
    } while (0)



#endif