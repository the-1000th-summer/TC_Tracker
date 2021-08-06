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
using System.ComponentModel;
using System.Diagnostics;
using Microsoft.WindowsAPICodePack.Dialogs;

using myCLI;
using System.Runtime.InteropServices;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged {

        private string cSelDir {
            get => Properties.Settings.Default.selectDir;
        }
        private string latVarStr;
        private string lonVarStr;
        private string vorVarStr;

        private bool _isTracking = false;
        public bool isTracking {
            get { return _isTracking; }
            set { 
                _isTracking = value;
                RaisePropertyChanged("isTracking");
            }
        }

        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private BackgroundWorker bgWorker;

        public event PropertyChangedEventHandler PropertyChanged;

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
            bgWorker = (BackgroundWorker)this.FindResource("backgroundWorker");

        }



        private void browseButton_Click(object sender, RoutedEventArgs e) {
            //ConsoleManager.Show();
            Console.WriteLine("browse button clicked!");

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
                NCFileInfo fileInfo = new NCFileInfo(selectDir, "", "", "");
                fileInfo.checkFileValid();
                Debug.WriteLine(fileInfo.fileValidInfo, " from cs.");
                if (!fileInfo.isFileValid) {
                    MessageBox.Show($"File\n{selectDir}\n is not valid:\n {fileInfo.fileValidInfo}");
                    selVarButton.IsEnabled = false;
                    latNameLabel.Content = "未指定";
                    lonNameLabel.Content = "未指定";
                    vorNameLabel.Content = "未指定";
                    return;
                }
                selVarButton.IsEnabled = true;
            }
        }

        private void exit_OnClick(object sender, RoutedEventArgs e) {
            //if (Properties.Settings.Default.isRunning) {
            if (isTracking) {
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
            latNameLabel.Content = latVarStr;
            lonNameLabel.Content = lonVarStr;
            vorNameLabel.Content = vorVarStr;
        }

        /// <summary>
        /// 点击了开始识别按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void startTrackButton_Click(object sender, RoutedEventArgs e) {
            //startTrackButton.IsEnabled = false;
            Console.WriteLine("sdffff");
            isTracking = true;

            bgWorker.RunWorkerAsync();

        }

        private void stopButtonClicked(object sender, RoutedEventArgs e) {
            Console.WriteLine("stop button clicked!");
            isTracking = false;
        }

        /// <summary>
        /// 分配到后台线程，从执行此方法开始
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bgWorker_DoWork(object sender, DoWorkEventArgs e) {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, latVarStr, lonVarStr, vorVarStr);
            fileInfo.startTracking();
        }

        /// <summary>
        /// 进度条值改变，执行此方法
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bgWorker_progressChanged(object sender, ProgressChangedEventArgs e) {

        }

        /// <summary>
        /// 后台线程执行完毕，执行此方法
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bgWorker_runCompleted(object sender, RunWorkerCompletedEventArgs e) {
            Console.WriteLine("run completed!");
        }

        /// <summary>
        /// 点击了设置按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void settingsClicked(object sender, RoutedEventArgs e) {
            var settingsView = new SettingsWindow();
            settingsView.Owner = this;
            settingsView.ShowDialog();
        }
    }
}
