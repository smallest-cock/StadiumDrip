#ifndef MACROS_H
#define MACROS_H


// convenient macros to avoid repetive typing  (should only be used within main plugin class)
// ... 'args' param comes last to support multiple variables in capture list


#define DELAY(delaySeconds, code) \
    gameWrapper->SetTimeout([this](GameWrapper* gw) { \
        code \
    }, delaySeconds)


#define DELAY_CAPTURE(delaySeconds, code, args) \
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


#define GAME_THREAD_EXECUTE_CAPTURE(code, args) \
    do { \
        gameWrapper->Execute([this, args](GameWrapper* gw) { \
            code \
        }); \
    } while (0)


#define DUMMY_THREAD(code) \
    do { \
        std::thread([this]() { \
            code \
        }).detach(); \
    } while (0)


#define DUMMY_THREAD_CAPTURE(code, args) \
    do { \
        std::thread([this, args]() { \
            code \
        }).detach(); \
    } while (0)



#endif