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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using Microsoft.WindowsAPICodePack.Dialogs;

using myCLI;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        private string cSelDir {
            get => Properties.Settings.Default.selectDir;
        }

        public MainWindow() {
            InitializeComponent();

            Entity e = new Entity("The Wallman", 20, 35);
            e.Move(5, -10);
            //label.Content = "sdf";
            //label.Content = e.XPosition + " " + e.YPosition;
            //Console.WriteLine(e.XPosition + " " + e.YPosition);
            //Console.Read();

        }

        private void browseButton_Click(object sender, RoutedEventArgs e) {
            var dialog = new CommonOpenFileDialog();

            var savedSDir = cSelDir;
            dialog.InitialDirectory = savedSDir == "" ? "C:\\Users" : savedSDir;
            dialog.IsFolderPicker = true;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
                var selectDir = dialog.FileName;
                Debug.WriteLine(selectDir);
                saveSelectDir(selectDir);
                //dirTextBox.Text = selectDir;
                //changeUIAccV(validateDir(dirTextBox.Text));
            }
        }

        private void exit_OnClick(object sender, RoutedEventArgs e) {
            Debug.WriteLine(sender.ToString());
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e) {
            //Debug
            Trace.WriteLine("ssdfsdf");
        }

        /// <summary>
        /// 保存工作文件夹字符串到settings中
        /// </summary>·
        /// <param name="text"></param>
        private void saveSelectDir(string text) {
            Properties.Settings.Default.selectDir = text;
            Properties.Settings.Default.Save();
        }
    }
}
