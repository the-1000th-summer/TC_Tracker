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
using TC_Tracker.otherWindow;

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
        private string step3FileDir {
            get => Properties.Settings.Default.step3FileDir;
            set {
                Properties.Settings.Default.step3FileDir = value;
                Properties.Settings.Default.Save();
            }
        }
        private string jsonFExportDir {
            get => Properties.Settings.Default.jsonFExportDir;
            set {
                Properties.Settings.Default.jsonFExportDir = value;
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

        private string timeVarStr = "";
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
        private bool _canStop = false;
        public bool canStop {
            get { return _canStop; }
            set {
                _canStop = value;
                RaisePropertyChanged("canStop");
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
        private bool _varNameSelected = false;
        public bool varNameSelected {
            get { return _varNameSelected; }
            set {
                _varNameSelected = value;
                RaisePropertyChanged("varNameSelected");
            }
        }
        private bool _zDimLvCanSelect = false;
        public bool zDimLvCanSelect {
            get { return _zDimLvCanSelect; }
            set {
                _zDimLvCanSelect = value;
                RaisePropertyChanged("zDimLvCanSelect");
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
            dialog.InitialDirectory = string.IsNullOrEmpty(savedSDir) ? "C:\\Users" : savedSDir;
            dialog.RestoreDirectory = true;
            //dialog.IsFolderPicker = true;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
                trackFinished = false;
                var filePath = dialog.FileName;
                Debug.WriteLine(filePath);

                ncFileTextBox.Text = filePath;
                //changeUIAccV(validateDir(dirTextBox.Text));
                if (!checkFileValidAndUpdateUI(filePath)) {
                    selectedFile = false;
                    return;
                }
                cSelDir = filePath;
                selectedFile = true;
                checkIsWrfoutFile();
            }
        }

        private bool checkFileValidAndUpdateUI(string selectDir) {
            NCFileInfo fileInfo = new NCFileInfo(selectDir, !isNotWrfoutFile, "", "", "", "", "");
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
            setVarNameLabel("未指定", "未指定", "未指定", "未指定");
        }

        private void checkIsWrfoutFile() {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, "", "", "", "", "");
            var exceptionInfo = "";
            var isWrfoutFile = fileInfo.checkIsWrfoutFile(ref exceptionInfo);

            if (isWrfoutFile) {
                isNotWrfoutFile = false;
                varNameSelected = true;
                //timeNameTextBlock.
                setVarNameLabel("XTIME", "XLAT", "XLONG", "---");
                handleZLevelDim();
            } else {
                isNotWrfoutFile = true;
                zDimLvCanSelect = false;
                zLvComboBox.SelectedIndex = -1;
                setVarNameLabel("未指定", "未指定", "未指定", "未指定");
            }
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
            if (!varSelView.selectOK) {
                varNameSelected = false;
                return;
            }
            timeVarStr = (string)varSelView.comboBox_time.SelectedValue;
            latVarStr = (string)varSelView.comboBox_lat.SelectedValue;
            lonVarStr = (string)varSelView.comboBox_lon.SelectedValue;
            vorVarStr = (string)varSelView.comboBox_vor.SelectedValue;
            setVarNameLabel(timeVarStr, latVarStr, lonVarStr, vorVarStr);
            handleZLevelDim();
            varNameSelected = true;

        }

        private void handleZLevelDim() {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            var zLvDimName = "";
            var zLvDimLen = fileInfo.getZLvDimLenName(ref zLvDimName);
            
            if (zLvDimLen == 0) {    // 无z维度
                zLvNameTextBlock.Text = "(无)";
                zLvComboBox.SelectedIndex = -1;
                zDimLvCanSelect = false;
                return;
            }
            zLvNameTextBlock.Text = zLvDimName;
            List<int> zLvIndex = Enumerable.Range(1, zLvDimLen).ToList();
            zLvComboBox.ItemsSource = zLvIndex;
            zLvComboBox.SelectedIndex = 0;
            zDimLvCanSelect = true;
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
            Console.WriteLine("start tracking.");
            isNotTracking = false;
            canStop = true;

            //bgWorker.RunWorkerAsync();

            asyncRun();
        }

        private void stopButtonClicked(object sender, RoutedEventArgs e) {
            Console.WriteLine("stop requested.");
            canStop = false;
            m_Cts.Cancel();
        }

        private async void asyncRun() {
            m_Cts = new System.Threading.CancellationTokenSource();
            m_Ct = m_Cts.Token;
            var selectedIndex = zLvComboBox.SelectedIndex;
            await Task.Factory.StartNew(() => {
                // Launching a cancelable operation performed by a managed C++Cli Object :
                Console.WriteLine(selectedIndex);
                NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, selectedIndex, s_TempFileDir, true);
                fileInfo.startTracking(realTCs, m_Ct);
                if (m_Ct.IsCancellationRequested) {
                    isNotTracking = true;
                    trackFinished = false;
                    return;
                }
                isNotTracking = true;
                trackFinished = true;
                canStop = false;
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
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            fileInfo.startFromStep2(realTCs);
            Console.WriteLine("msg from step2ButtonClick, realTCs number: {0}", realTCs.Count);
        }

        private void step3Button_Click(object sender, RoutedEventArgs e) {
            realTCs.Clear();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            fileInfo.startFromStep3(realTCs);
            Console.WriteLine("msg from step3ButtonClick, realTCs number: {0}", realTCs.Count);
        }

        private void setVarNameLabel(string timeLabelName, string latLabelName, string lonLabelName, string varNameLabelName) {
            timeNameTextBlock.Text = timeLabelName;
            latNameTextBlock.Text = latLabelName;
            lonNameTextBlock.Text = lonLabelName;
            vorNameTextBlock.Text = varNameLabelName;
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

        private void exportFile(bool selStep3FileFirst, string step3FilePath="") {
            Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
            dlg.InitialDirectory = string.IsNullOrEmpty(jsonFExportDir) ? "C:\\Users" : jsonFExportDir;
            dlg.RestoreDirectory = true;
            dlg.FileName = "Document"; // Default file name
            dlg.DefaultExt = ".text"; // Default file extension
            dlg.Filter = "json files|*.json"; // Filter files by extension

            Nullable<bool> result = dlg.ShowDialog();

            // Process save file dialog box results
            if (result == true) {
                // Save document
                jsonFExportDir = System.IO.Path.GetDirectoryName(dlg.FileName);
                string outFilePath = dlg.FileName;
                Console.WriteLine("filterindex: {0}", dlg.FilterIndex);
                Console.WriteLine("filePath: {0}", outFilePath);

                NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, "", "", "", "", s_TempFileDir);
                if (selStep3FileFirst) {
                    fileInfo.exportFile(step3FilePath, outFilePath);
                } else {
                    fileInfo.exportFile(outFilePath);
                }
            }
        }

        private void exportButton_Click(object sender, RoutedEventArgs e) {
            exportFile(false);
        }

        private void menu_exportJson_Click(object sender, RoutedEventArgs e) {
            var dialog = new CommonOpenFileDialog();
            dialog.InitialDirectory = string.IsNullOrEmpty(step3FileDir) ? "C:\\Users" : step3FileDir;
            dialog.RestoreDirectory = true;
            if (dialog.ShowDialog() != CommonFileDialogResult.Ok)
                return;
            step3FileDir = System.IO.Path.GetDirectoryName(dialog.FileName);
            exportFile(true, dialog.FileName);
        }

        private void menu_drawStep3_Click(object sender, RoutedEventArgs e) {
            var dialog = new CommonOpenFileDialog();
            dialog.InitialDirectory = string.IsNullOrEmpty(step3FileDir) ? "C:\\Users" : step3FileDir;
            dialog.RestoreDirectory = true;
            if (dialog.ShowDialog() != CommonFileDialogResult.Ok)
                return;
            step3FileDir = System.IO.Path.GetDirectoryName(dialog.FileName);

            var tcs = new List<Typhoon>();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, "", "", "", "", s_TempFileDir);
            fileInfo.getDataFromStep3File(dialog.FileName, tcs);

            var resultView = new ResultWindow();
            //resultView.tcsData = realTCs.GetRange(0,1);
            resultView.tcsData = tcs;
            resultView.Owner = this;
            resultView.ShowDialog();
        }

        private void button1_Click(object sender, RoutedEventArgs e) {
            var paraSelWin = new ParaSelWindow();
            paraSelWin.Owner = this;
            paraSelWin.ShowDialog();
        }
    }
}
