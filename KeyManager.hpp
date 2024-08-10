#pragma once
#include <array>
#include <atomic>
#include <basetsd.h>
#include <memory>
#include <unordered_map>
#include <Windows.h>

namespace KeyManagement {

    class Utils {
    public:
        static constexpr INPUT createMouseInput(DWORD flags, LONG dx = 0, LONG dy = 0, DWORD mouseData = 0);
        static constexpr INPUT createKeyboardInput(WORD key, DWORD flags = 0);
        static const INPUT& getLeftDown();
        static const INPUT& getLeftUp();
        static const INPUT& getRDown();
        static const INPUT& getRUp();
        static const INPUT& getFDown();
        static const INPUT& getFUp();
        static const INPUT& getAntiDrift();
        static void delay(double milliseconds);
        static void createBeepProcess(int frequency, int duration);
        static bool SetProcessAffinity(DWORD processId, DWORD_PTR affinityMask);        
    };

    class KeyManager {
    public:
        virtual void keyHandlerProc(int keyCode);
        static void dispatchKey(int keyCode);
        static void registerKey(int keyCode, std::unique_ptr<KeyManager> manager);
        static bool getKeyState(int keyCode);
        static void setKeyState(int keyCode, bool state);

    private:
        static std::atomic<bool> keyStates[256];
        static std::unordered_map<int, std::unique_ptr<KeyManager>> keyManagerTable;
    };

    class Spin : public KeyManager {
    public:
        virtual void keyHandlerProc(int keyCode) override;
        Spin();
        static void update(const int dx, const int dy);
        static void reset();
        static auto get();

    private:
        static const INPUT defaultInput;
        static std::atomic<INPUT> input;
        static void show();
    };

    class Pick : public KeyManager {
    public:
        Pick();
        virtual void keyHandlerProc(int keyCode) override;
    private:
        const std::array<INPUT, 6> pick;
    };

    class Click : public KeyManager {
    public:
        virtual void keyHandlerProc(int keyCode) override;
    };

    class MachineGun : public KeyManager {
    public:
        virtual void keyHandlerProc(int keyCode) override;
    };

} // namespace KeyManagement