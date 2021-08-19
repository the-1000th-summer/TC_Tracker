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
            set {
                Properties.Settings.Default.selectDir = value;
                Properties.Settings.Default.Save();
            }
        }
        private string s_TempFileDir {
            get => Properties.Settings.Default.tempFileDir;
            set {
                Properties.Settings.Default.tempFileDir = value;
                Properties.Settings.Default.Save();
            }
        }

        private string latVarStr = "";
        private string lonVarStr = "";
        private string vorVarStr = "";

        private bool _isNotTracking = true;
        public bool isNotTracking {
            get { return _isNotTracking; }
            set {
                _isNotTracking = value;
                RaisePropertyChanged("isNotTracking");
            }
        }
        private bool _selectedFile = false;
        public bool selectedFile {
            get { return _selectedFile; }
            set {
                _selectedFile = value;
                RaisePropertyChanged("selectedFile");
            }
        }
        private bool _trackFinished = false;
        public bool trackFinished {
            get { return _trackFinished; }
            set {
                _trackFinished = value;
                RaisePropertyChanged("trackFinished");
            }
        }
        private bool _varNameSelected = false;
        public bool varNameSelected {
            get { return _varNameSelected; }
            set {
                _varNameSelected = value;
                RaisePropertyChanged("varNameSelected");
            }
        }

        private bool _isNotWrfoutFile = true;
        public bool isNotWrfoutFile {
            get { return _isNotWrfoutFile; }
            set {
                _isNotWrfoutFile = value;
                RaisePropertyChanged("isNotWrfoutFile");
            }
        }

        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private BackgroundWorker bgWorker;
        private List<Typhoon> realTCs = new List<Typhoon>();

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

                ncFileTextBox.Text = selectDir;
                //changeUIAccV(validateDir(dirTextBox.Text));
                if (!checkFileValidAndUpdateUI(selectDir)) {
                    selectedFile = false;
                    return;
                }
                cSelDir = selectDir;
                selectedFile = true;
            }
        }

        private bool checkFileValidAndUpdateUI(string selectDir) {
            NCFileInfo fileInfo = new NCFileInfo(selectDir, !isNotWrfoutFile, "", "", "", "");
            var fileValidInfo = "";
            var isFileValid = fileInfo.checkFileValid(ref fileValidInfo);
            Debug.WriteLine(fileValidInfo, " from cs.");
            if (!isFileValid) {
                MessageBox.Show($"File\n{selectDir}\n is not valid:\n {fileValidInfo}");
                notValidUI();
                return false;
            }
            return true;
        }

        private void notValidUI() {
            selVarButton.IsEnabled = false;
            setVarNameLabel("未指定", "未指定", "未指定");
        }

        /// <summary>
        /// 保存工作文件夹字符串到settings中
        /// </summary>
        /// <param name="text"></param>
        //private void saveSelectDir(string text) {
        //    Properties.Settings.Default.selectDir = text;
        //    Properties.Settings.Default.Save();
        //}

        /// <summary>
        /// 点击选择变量按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void varSelClicked(object sender, RoutedEventArgs e) {
            var varSelView = new VarSelector();
            varSelView.Owner = this;
            varSelView.ShowDialog();
            if (!varSelView.selectOK) {
                varNameSelected = false;
                return;
            }

            latVarStr = (string)varSelView.comboBox_lat.SelectedValue;
            lonVarStr = (string)varSelView.comboBox_lon.SelectedValue;
            vorVarStr = (string)varSelView.comboBox_vor.SelectedValue;
            latNameLabel.Content = latVarStr;
            lonNameLabel.Content = lonVarStr;
            vorNameLabel.Content = vorVarStr;
            varNameSelected = true;
        }

        private System.Threading.CancellationTokenSource m_Cts;
        private System.Threading.CancellationToken m_Ct;

        /// <summary>
        /// 点击了开始识别按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void startTrackButton_Click(object sender, RoutedEventArgs e) {
            //startTrackButton.IsEnabled = false;
            Console.WriteLine("stack tracking.");
            isNotTracking = false;

            //bgWorker.RunWorkerAsync();

            syncRun();
        }

        private void stopButtonClicked(object sender, RoutedEventArgs e) {
            Console.WriteLine("stop!");
            m_Cts.Cancel();
            isNotTracking = true;
            trackFinished = false;
        }

        private async void syncRun() {
            m_Cts = new System.Threading.CancellationTokenSource();
            m_Ct = m_Cts.Token;

            await Task.Factory.StartNew(() => {
                // Launching a cancelable operation performed by a managed C++Cli Object :
                NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, latVarStr, lonVarStr, vorVarStr, s_TempFileDir, true);
                fileInfo.startTracking(realTCs, m_Ct);
                isNotTracking = true;
                trackFinished = true;
            });

            
        }

        /// <summary>
        /// 分配到后台线程，从执行此方法开始
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bgWorker_DoWork(object sender, DoWorkEventArgs e) {
            //NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            //fileInfo.startTracking(realTCs);
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
            isNotTracking = true;
            trackFinished = true;
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

        private void about_OnClick(object sender, RoutedEventArgs e) {
            bool isWindowOpen = false;
            foreach (Window w in Application.Current.Windows) {
                if (w is AboutWindow) {
                    isWindowOpen = true;
                    w.Activate();
                }
            }
            if (!isWindowOpen) {
                var newwindow = new AboutWindow();
                newwindow.Show();
            }
        }

        private void showWebButton_Click(object sender, RoutedEventArgs e) {
            var resultView = new ResultWindow();
            //resultView.tcsData = realTCs.GetRange(0,1);
            resultView.tcsData = realTCs;
            resultView.Owner = this;
            resultView.ShowDialog();
        }
        
        /// <summary>
        /// Drag and drop放手时执行的方法
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void filePathDrop(object sender, DragEventArgs e) {
            // 可能drop多个文件
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            if (files.Length != 1) {
                MessageBox.Show("选择的文件多于一个，请重新选择。");
                notValidUI();
                return;
            }
            string fileStr = files[0];
            Console.WriteLine(fileStr);
            ncFileTextBox.Text = fileStr;
            if (!checkFileValidAndUpdateUI(fileStr)) { return; }
            cSelDir = fileStr;
            selVarButton.IsEnabled = true;
        }

        private void step2Button_Click(object sender, RoutedEventArgs e) {
            realTCs.Clear();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            fileInfo.startFromStep2(realTCs);
            Console.WriteLine("msg from step2ButtonClick, realTCs number: {0}", realTCs.Count);
        }

        private void step3Button_Click(object sender, RoutedEventArgs e) {
            realTCs.Clear();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            fileInfo.startFromStep3(realTCs);
            Console.WriteLine("msg from step3ButtonClick, realTCs number: {0}", realTCs.Count);
        }

        private void wrfoutCheckBox_Checked(object sender, RoutedEventArgs e) {
            isNotWrfoutFile = false;
            varNameSelected = true;
            setVarNameLabel("XLAT", "XLONG", "---");
        }
        private void wrfoutCheckBox_Unchecked(object sender, RoutedEventArgs e) {
            isNotWrfoutFile = true;
            varNameSelected = false;
            latVarStr = lonVarStr = vorVarStr = "";
            setVarNameLabel("未指定", "未指定", "未指定");
        }

        private void setVarNameLabel(string latLabelName, string lonLabelName, string varNameLabelName) {
            latNameLabel.Content = latLabelName;
            lonNameLabel.Content = lonLabelName;
            vorNameLabel.Content = varNameLabelName;
        }

        private void exit_OnClick(object sender, RoutedEventArgs e) {
            //if (Properties.Settings.Default.isRunning) {
            if (!isNotTracking) {
                if (MessageBox.Show("退出程序吗？任务仍在执行。", "退出程序吗？", MessageBoxButton.YesNo, MessageBoxImage.Question) ==
                    MessageBoxResult.No) {
                    return;
                }
            }
            Application.Current.Shutdown();
        }

        private void windowClosing(object sender, CancelEventArgs e) {
            if (!isNotTracking) {
                if (MessageBox.Show("退出程序吗？任务仍在执行。", "退出程序吗？", MessageBoxButton.YesNo, MessageBoxImage.Question) ==
                    MessageBoxResult.No) {
                    e.Cancel = true;
                    return;
                }
            }
        }

        private void exportButton_Click(object sender, RoutedEventArgs e) {
            Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
            dlg.FileName = "Document"; // Default file name
            dlg.DefaultExt = ".text"; // Default file extension
            dlg.Filter = "json files (.json)|*.json"; // Filter files by extension

            Nullable<bool> result = dlg.ShowDialog();

            // Process save file dialog box results
            if (result == true) {
                // Save document
                string filename = dlg.FileName;
                Console.WriteLine("filterindex: {0}", dlg.FilterIndex);
                Console.WriteLine("FIlename: {0}", filename);
            }
        }
    }
}
