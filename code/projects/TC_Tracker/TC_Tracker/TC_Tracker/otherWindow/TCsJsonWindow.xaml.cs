using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for TCsJsonWindow.xaml
    /// </summary>
    public partial class TCsJsonWindow : Window {
        DispatcherTimer timer;

        public TCsJsonWindow() {
            InitializeComponent();
        }

        private void copyButton_Click(object sender, RoutedEventArgs e) {
            Clipboard.SetText(jsonTextBox.Text);
            label.Visibility = Visibility.Visible;
            copyButton.IsEnabled = false;

            timer = new DispatcherTimer();
            timer.Tick += new EventHandler(timer_Tick);
            timer.Interval = new TimeSpan(0, 0, 1);
            timer.Start();
        }

        private void timer_Tick(object sender, EventArgs e) {
            label.Visibility = Visibility.Collapsed;
            copyButton.IsEnabled = true;
            timer.Stop();
        }
    }
}
