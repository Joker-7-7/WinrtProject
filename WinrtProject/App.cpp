#include "pch.h"
#include "Game.h"
// test comment
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
        //������������ ����������
        //CoreApplication::Suspending({ this, &App::OnSuspending });
        //�������� ����������
        //CoreApplication::Resuming({ this, &App::OnResuming });
    }

    void Load(hstring const&)
    {
    }

    void Uninitialize()
    {
        Game.Uninitialize();
    }

    void Run()
    {
        Game.Initialize();

        CoreWindow window = CoreWindow::GetForCurrentThread();

        CoreDispatcher dispatcher = window.Dispatcher();
        while (!WindowClosed)
        {
            dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

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
        if (args.VirtualKey() == VirtualKey::Up)
        {
            Game.heightZ += Game.deltaZ;
        }
        if (args.VirtualKey() == VirtualKey::Down)
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
