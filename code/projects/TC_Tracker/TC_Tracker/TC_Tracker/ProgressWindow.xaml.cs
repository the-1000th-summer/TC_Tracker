using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;
using myCLI;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for ProgressWindow.xaml
    /// </summary>
    public partial class ProgressWindow : Window {

        private NCFileInfo tracker;
        private List<Typhoon> realTCs = new List<Typhoon>();

        private System.Threading.CancellationTokenSource m_Cts;
        private System.Threading.CancellationToken m_Ct;

        private string aa = "label";

        public ProgressWindow() {
            InitializeComponent();
        }

        public ProgressWindow(NCFileInfo tracker): this() {
            this.tracker = tracker;
            //var process = new Progress<string>((p) => {
            //    label.Content = label.Content + "!";
            //});
            asyncRun();
        }

        private async void asyncRun() {
            m_Cts = new System.Threading.CancellationTokenSource();
            m_Ct = m_Cts.Token;

            await Task.Factory.StartNew(() => {
                tracker.startTracking(realTCs, m_Ct, myCallBack, myCallBack2);
            });
        }

        private void myCallBack(int a) {
            Dispatcher.BeginInvoke(DispatcherPriority.Input, new ThreadStart(() => {
                label.Content = label.Content + "!";
            }));
            //label.Content = label.Content + "!";
            //aa += "!";
            //progress.Report(aa);
            Console.Write("In my CallBack: ");
            Console.WriteLine(a);
        }

        private void myCallBack2(double a) {
            Dispatcher.BeginInvoke(DispatcherPriority.Input, new ThreadStart(() => {
                label.Content = label.Content + "!";
            }));
            //label.Content = label.Content + "!";
            //aa += "!";
            //progress.Report(aa);
            Console.Write("In my CallBack2: ");
            Console.WriteLine(a);
        }

    }
}
