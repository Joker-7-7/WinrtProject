#include "pch.h"
#include "Game.h"

using namespace winrt;
using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::ApplicationModel;
using namespace Windows::Graphics::Display;

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    bool WindowClosed;
    CGame Game;
    IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(CoreApplicationView const& applicationView)
    {
        applicationView.Activated({ this, &App::OnActivated });
        WindowClosed = false;    // initialize to false
        //приостановка приложения
        //CoreApplication::Suspending({ this, &App::OnSuspending });
        //загрузка приложения
        //CoreApplication::Resuming({ this, &App::OnResuming });
    }

    void Load(hstring const&)
    {
    }

    void Uninitialize()
    {
    }

    void Run()
    {
        Game.Initialize();

        //static DWORD previousTime = time(NULL);
        //static const float targetFramerate = 30.0f;
        //static const float maxTimeStep = 1.0f / targetFramerate;

        CoreWindow window = CoreWindow::GetForCurrentThread();

        CoreDispatcher dispatcher = window.Dispatcher();
        while (!WindowClosed)
        {
            dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            //DWORD currentTime = time(NULL);
            //float deltaTime = (currentTime - previousTime) / 1000.0f;
            //previousTime = currentTime;

            // Cap the delta time to the max time step (useful if your 
            // debugging and you don't want the deltaTime value to explode.
            //deltaTime = std::min<float>(deltaTime, maxTimeStep);

            Game.Update();
            Game.Render();
        }
    }

    void SetWindow(CoreWindow const & window)
    {
        //window.PointerPressed({ this, &App::OnPointerPressed });
        window.KeyDown({ this, &App::OnPressedKeyDown });
        //window.KeyUp({ this, &App::OnPressedKeyUp });
        window.Closed({ this, &App::OnClosed });
    }

    void OnActivated(CoreApplicationView const& /* applicationView */, IActivatedEventArgs const& /* args */)
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();
    }

    void OnSuspending(IInspectable const& /* sender */, SuspendingEventArgs const& args)
    {

    }

    void OnResuming(IInspectable const& /* sender */, IInspectable const& /* args */)
    {
    }

    void OnPointerPressed(CoreWindow const&, PointerEventArgs const & args)
    {
        MessageDialog  Dialog(L"Thank you for noticing this notice.", L"Notice!");
        Dialog.ShowAsync();
    }

    void OnPressedKeyDown(CoreWindow const&, KeyEventArgs const& args)
    {
        if (args.VirtualKey() == VirtualKey::Escape)
        {
            WindowClosed = true;
        }
        if (args.VirtualKey() == VirtualKey::A)
        {
            Game.angle += 90.0f * Game.deltaTime;
        }
        if (args.VirtualKey() == VirtualKey::D)
        {
            Game.angle -= 90.0f * Game.deltaTime;
        }
        if (args.VirtualKey() == VirtualKey::W)
        {
            Game.heightZ += Game.deltaZ;
        }
        if (args.VirtualKey() == VirtualKey::S)
        {
            Game.heightZ -= Game.deltaZ;
        }
    }
    void OnPressedKeyUp(CoreWindow const&, KeyEventArgs const& args)
    {

    }

    void OnClosed(CoreWindow const& sender, CoreWindowEventArgs const& args)
    {
        WindowClosed = true;
    }

    void OnPointerMoved(IInspectable const &, PointerEventArgs const & args)
    {

    }

    void AddVisual(float2 const point)
    {

    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<App>());
}
