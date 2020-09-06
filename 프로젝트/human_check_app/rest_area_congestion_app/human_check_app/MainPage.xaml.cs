using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using Tizen.Applications.Messages;
using Tizen; //For dlog




namespace rest_area_congestion_app
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class MainPage : ContentPage
    {
        public static MainPage instance;

        private bool isFal = false;
        private int cnt = 0;

        public static MessagePort _msgPort;
        private readonly string TAG = "MessagePort";


        int rest_area_congestion = 0; // 통신 데이터 저장
        int index = 0;
        private string[] maxims = new string[]
        {
            "여  유",
            "약간 혼잡",
            "혼  잡"
        };

        private string[] maximimg = new string[]
        {
            "good.png",
            "soso.png",
            "bad.png"
        };
        public MainPage()
        {

            InitializeComponent();


            _msgPort = new MessagePort("sample_ui_port", false);
            _msgPort.MessageReceived += MessageReceived_Callback;

            _msgPort.Listen();

        }

        private void Text_state(object sender, EventArgs e/*, FileImageSource Image*/)
        {

            image.Source = maximimg[index];
            good.Text = maxims[index++];

            if (index >= maxims.Length)
                index = 0;

        }

           private void MessageReceived_Callback(object sender, MessageReceivedEventArgs e)
        {
            Log.Debug(TAG, "Message Received");
            Log.Debug(TAG, "App ID: " + e.Remote.AppId);
            Log.Debug(TAG, "PortName: " + e.Remote.PortName);
            Log.Debug(TAG, "Trusted: " + e.Remote.Trusted);
            Log.Debug(TAG, "message: " + e.Message.GetItem<string>("response data"));
        }



        void OnButtonClicked(object sender, EventArgs args)
        {
            cnt++;
            isFal = true;
            while (isFal)
            {
                if (cnt == 2)
                {
                    isFal = false;
                    cnt = 0;
                }
                string remoteAppId = "rest_area_congestion_service";
                string remotePort = "rest_area_congestion_service_port";

                var msg = new Tizen.Applications.Bundle();
                msg.AddItem("data", "Send_A_MESSAGE_TO_A_REMOTE_APP");
                _msgPort.Send(msg, remoteAppId, remotePort);
           }
        }
    }
}