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
        private string latVarStr;
        private string lonVarStr;
        private string vorVarStr;

        public MainWindow() {
            InitializeComponent();

            ncFileTextBox.Text = cSelDir;

            Properties.Settings.Default.isRunning = false;
            Properties.Settings.Default.Save();

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
            //dialog.is
            //dialog.IsFolderPicker = true;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
                var selectDir = dialog.FileName;
                Debug.WriteLine(selectDir);
                saveSelectDir(selectDir);
                ncFileTextBox.Text = selectDir;
                //changeUIAccV(validateDir(dirTextBox.Text));
                NCFileInfo fileInfo = new NCFileInfo(selectDir);
                fileInfo.checkFileValid();
                Debug.WriteLine(fileInfo.fileValidInfo, " from cs.");
                if (!fileInfo.isFileValid) {
                    MessageBox.Show($"File\n{selectDir}\n is not valid:\n {fileInfo.fileValidInfo}");
                    selVarButton.IsEnabled = false;
                    return;
                }
                selVarButton.IsEnabled = true;
            }
        }

        private void exit_OnClick(object sender, RoutedEventArgs e) {
            if (Properties.Settings.Default.isRunning) {
                if (MessageBox.Show("退出程序吗？任务仍在执行。", "退出程序吗？", MessageBoxButton.YesNo, MessageBoxImage.Question) ==
                    MessageBoxResult.No) {
                    return;
                }
            }
            Application.Current.Shutdown();
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e) {
            //Debug
            Trace.WriteLine("ssdfsdf");
        }

        /// <summary>
        /// 保存工作文件夹字符串到settings中
        /// </summary>
        /// <param name="text"></param>
        private void saveSelectDir(string text) {
            Properties.Settings.Default.selectDir = text;
            Properties.Settings.Default.Save();
        }

        /// <summary>
        /// 点击选择变量按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void varSelClicked(object sender, RoutedEventArgs e) {
            var varSelView = new VarSelector();
            varSelView.Owner = this;
            varSelView.ShowDialog();
            if (!varSelView.selectOK) { return; }

            latVarStr = (string)varSelView.comboBox_lat.SelectedValue;
            lonVarStr = (string)varSelView.comboBox_lon.SelectedValue;
            vorVarStr = (string)varSelView.comboBox_vor.SelectedValue;
        }
    }
}
