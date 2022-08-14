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
        private string jsonFExportDir {
            get => Properties.Settings.Default.jsonFExportDir;
            set {
                Properties.Settings.Default.jsonFExportDir = value;
                Properties.Settings.Default.Save();
            }
        }
        //private string step3FileDir {
        //    get => Properties.Settings.Default.step3FileDir;
        //    set {
        //        Properties.Settings.Default.step3FileDir = value;
        //        Properties.Settings.Default.Save();
        //    }
        //}
        //private string jsonFExportDir {
        //    get => Properties.Settings.Default.jsonFExportDir;
        //    set {
        //        Properties.Settings.Default.jsonFExportDir = value;
        //        Properties.Settings.Default.Save();
        //    }
        //}
        //private string s_TempFileDir {
        //    get => Properties.Settings.Default.tempFileDir;
        //    set {
        //        Properties.Settings.Default.tempFileDir = value;
        //        Properties.Settings.Default.Save();
        //    }
        //}

        private string _timeVarStr = "未指定";
        public string timeVarStr {
            get => _timeVarStr;
            set {
                _timeVarStr = value;
                RaisePropertyChanged(nameof(timeVarStr));
            }
        }
        private string _latVarStr = "未指定";
        public string latVarStr {
            get => _latVarStr;
            set {
                _latVarStr = value;
                RaisePropertyChanged(nameof(latVarStr));
            }
        }
        private string _lonVarStr = "未指定";
        public string lonVarStr {
            get => _lonVarStr;
            set {
                _lonVarStr = value;
                RaisePropertyChanged(nameof(lonVarStr));
            }
        }
        private string _vorVarStr = "未指定";
        public string vorVarStr {
            get => _vorVarStr;
            set {
                _vorVarStr = value;
                vorLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged(nameof(vorVarStr));
            }
        }
        private string _uwndVarStr = "未指定";
        public string uwndVarStr {
            get => _uwndVarStr;
            set {
                _uwndVarStr = value;
                uwndLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged(nameof(uwndVarStr));
            }
        }
        private string _vwndVarStr = "未指定";
        public string vwndVarStr {
            get => _vwndVarStr;
            set {
                _vwndVarStr = value;
                vwndLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged(nameof(vwndVarStr));
            }
        }

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
        private bool _trackFinished = false;
        public bool trackFinished {
            get { return _trackFinished; }
            set {
                _trackFinished = value;
                RaisePropertyChanged("trackFinished");
            }
        }
        private bool isWrfoutFile = false;
        //public bool isWrfoutFile {
        //    get { return _isWrfoutFile; }
        //    set {
        //        _isWrfoutFile = value;
        //        RaisePropertyChanged(nameof(isWrfoutFile));
        //    }
        //}

        private bool _shouldInterp = false;
        public bool shouldInterp {
            get => _shouldInterp;
            set {
                _shouldInterp = value;
                RaisePropertyChanged("shouldInterp");
            }
        }
        public Visibility interpTextBoxVisibility {
            get {
                return shouldInterp ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public string interpLabelStr {
            get {
                return shouldInterp ? "插值到格点分辨率：" : "不插值";
            }
        }


        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private int zLvDimLen = 0;
        private List<Typhoon> realTCs = new List<Typhoon>();

        //private BackgroundWorker bgWorker;
        //private List<Typhoon> realTCs = new List<Typhoon>();

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindow() {
            InitializeComponent();

            Properties.Settings.Default.isRunning = false;
            Properties.Settings.Default.Save();

            PropertyChanged += mainWin_PropertyChanged;
        }

        private void browseButtonClicked(object sender, RoutedEventArgs e) {
            Console.WriteLine("browse button clicked!");

            var filePath = showFileBrowser();
            if (filePath == "") { return; }

            cSelDir = filePath;
            zLvComboBox.IsEnabled = false;
            showWebBtn.IsEnabled = false;
            canceledLabel.Visibility = Visibility.Collapsed;

            if (!checkFileValidAndUpdateUI(filePath)) {
                interpCheckBox.IsEnabled = false;
                return;
            }

            interpCheckBox.IsEnabled = true;
            zLvNameTextBlock.Content = "";

            checkIfIsWrfoutFile();
        }

        private String showFileBrowser() {
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

                return filePath;
            }
            return "";
        }

        private bool checkFileValidAndUpdateUI(string selectDir) {
            NCFileInfo fileInfo = new NCFileInfo(selectDir);
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
            selVarNameBtn.IsEnabled = false;
            setVarName("未指定");
        }

        private void checkIfIsWrfoutFile() {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir);
            var exceptionInfo = "";
            isWrfoutFile = fileInfo.checkIsWrfoutFile(ref exceptionInfo);

            if (isWrfoutFile) {
                
                //timeNameTextBlock.
                setVarName("XTIME", "XLAT", "XLONG", "", "U", "V");
                handleZLevelDim();

                selVarNameBtn.IsEnabled = false;
                startTrackingBtn.IsEnabled = true;
                zLvComboBox.IsEnabled = true;

            } else {

                zLvComboBox.SelectedIndex = -1;
                setVarName("未指定");

                selVarNameBtn.IsEnabled = true;
            }
        }

        private void setVarName(string time, string lat, string lon, string vor, string u, string v) {
            timeVarStr = time;
            latVarStr = lat;
            lonVarStr = lon;
            vorVarStr = vor;
            uwndVarStr = u;
            vwndVarStr = v;
        }

        private void setVarName(string allStr) {
            setVarName(allStr, allStr, allStr, allStr, allStr, allStr);
        }

        /// <summary>
        /// 点击选择变量按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void varSelClicked(object sender, RoutedEventArgs e) {
            var varSelWindow = new VarSelectWindow();
            varSelWindow.Owner = this;
            varSelWindow.onVarNamesSend += getVarNames;
            varSelWindow.ShowDialog();

        }

        private void getVarNames(List<string> varNames) {
            setVarName(varNames[0], varNames[1], varNames[2], varNames[3], varNames[4], varNames[5]);

            handleZLevelDim();

            startTrackingBtn.IsEnabled = true;
        }

        private void handleZLevelDim() {
            var zLvDimName = "";
            zLvDimLen = (new NCFileInfo(cSelDir, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !string.IsNullOrEmpty(vorVarStr))).getZLvDimLenName(ref zLvDimName);

            zLvComboBox.SelectedIndex = -1;

            if (zLvDimLen == 0) {    // 无z维度
                zLvNameTextBlock.Content = "(无)";
                zLvNameTextBlock.IsEnabled = false;
                return;
            }
            zLvComboBox.IsEnabled = true;
            zLvNameTextBlock.Content = zLvDimName;
            List<int> zLvIndex = Enumerable.Range(0, zLvDimLen).ToList();

            zLvComboBox.ItemsSource = zLvIndex;
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

        private void mainWin_PropertyChanged(object sender, PropertyChangedEventArgs e) {
            if (e.PropertyName == nameof(shouldInterp)) {
                RaisePropertyChanged(nameof(interpTextBoxVisibility));
                RaisePropertyChanged(nameof(interpLabelStr));
            }
            
        }

        private void startTrackBtnClicked(object sender, RoutedEventArgs e) {
            if (!checkZLvComboBox()) { return; }

            (var checkPassed, var gridResValue) = checkGridResValue();
            if (!checkPassed) { return; }
            selVarNameBtn.IsEnabled = false;
            startTrackingBtn.IsEnabled = false;
            zLvComboBox.IsEnabled = false;

            var tracker = new NCFileInfo(cSelDir, isWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !string.IsNullOrEmpty(vorVarStr), (zLvDimLen == 0) ? -1 : Int32.Parse(zLvComboBox.SelectedItem.ToString()), 4, gridResValue, "E:\\University\\TC_Tracker\\data\\out");

            var progressWin = new ProgressWindow(tracker);
            progressWin.Owner = this;
            progressWin.ShowDialog();
        }

        private bool checkZLvComboBox() {
            if (zLvDimLen == 0) { return true; }
            var selectedIndex = zLvComboBox.SelectedIndex;
            if (selectedIndex == -1) {
                MessageBox.Show("层数不能为空！");
                return false;
            }
            return true;
        }

        private Tuple<bool, double> checkGridResValue() {
            if (!(interpCheckBox.IsChecked ?? false)) {
                return Tuple.Create(true, 0.0);
            }
            var gridResStr = gridResTextBox.Text;
            if (string.IsNullOrEmpty(gridResStr)) {
                MessageBox.Show("格点分辨率不能为空！");
                return Tuple.Create(false, 0.0);
            }
            if (!double.TryParse(gridResStr, out var gridRes)) {
                MessageBox.Show("输入的格点分辨率不合法。");
                return Tuple.Create(false, 0.0);
            }
            return Tuple.Create(true, gridRes);
        }

        public void setRealTCs(List<Typhoon> tcs) {
            realTCs = tcs;
        }

        private void showWebBtnClicked(object sender, RoutedEventArgs e) {
            var resultView = new ResultWindow(realTCs);
            //resultView.tcsData = realTCs.GetRange(0,1);
            //resultView.tcsData = realTCs;
            resultView.Owner = this;
            resultView.ShowDialog();
        }

        private void gridResTextBox_PreviewTextInput(object sender, TextCompositionEventArgs e) {
            if (!(double.TryParse(((TextBox)sender).Text + e.Text, out _))) {
                e.Handled = true;
                System.Media.SystemSounds.Asterisk.Play();
            }
        }

        private void gridResTextBox_PreviewKeyDown(object sender, KeyEventArgs e) {
            // Prohibit space in textbox
            if (e.Key == Key.Space) {
                e.Handled = true;
                System.Media.SystemSounds.Asterisk.Play();
            }
        }

        private void exportBtnClicked(object sender, RoutedEventArgs e) {
            exportFile(false);
        }

        private void exportFile(bool selStep3FileFirst, string step3FilePath = "") {
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

                //NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, "", "", "", "", s_TempFileDir);
                //if (selStep3FileFirst) {
                //    fileInfo.exportFile(step3FilePath, outFilePath);
                //} else {
                //    fileInfo.exportFile(outFilePath);
                //}
            }
        }
    }
}
