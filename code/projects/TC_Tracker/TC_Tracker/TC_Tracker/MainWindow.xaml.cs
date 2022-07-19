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
                RaisePropertyChanged("latVarStr");
            }
        }
        private string _latVarStr = "未指定";
        public string latVarStr {
            get => _latVarStr;
            set {
                _latVarStr = value;
                RaisePropertyChanged("latVarStr");
            }
        }
        private string _lonVarStr = "未指定";
        public string lonVarStr {
            get => _lonVarStr;
            set {
                _lonVarStr = value;
                RaisePropertyChanged("lonVarStr");
            }
        }
        private string _vorVarStr = "未指定";
        public string vorVarStr {
            get => _vorVarStr;
            set {
                _vorVarStr = value;
                vorLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged("vorVarStr");
            }
        }
        private string _uwndVarStr = "未指定";
        public string uwndVarStr {
            get => _uwndVarStr;
            set {
                _uwndVarStr = value;
                uwndLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged("uwndVarStr");
            }
        }
        private string _vwndVarStr = "未指定";
        public string vwndVarStr {
            get => _vwndVarStr;
            set {
                _vwndVarStr = value;
                vwndLabel.Foreground = string.IsNullOrEmpty(value) ? Brushes.Gray : Brushes.Black;
                RaisePropertyChanged("vwndVarStr");
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

        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        //private BackgroundWorker bgWorker;
        //private List<Typhoon> realTCs = new List<Typhoon>();

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindow() {
            InitializeComponent();

            Properties.Settings.Default.isRunning = false;
            Properties.Settings.Default.Save();

            PropertyChanged += mainWin_PropertyChanged;
        }

        private void browseButton_Click(object sender, RoutedEventArgs e) {
            Console.WriteLine("browse button clicked!");

            var filePath = showFileBrowser();
            if (filePath == "") { return; }

            cSelDir = filePath;
            zLvComboBox.IsEnabled = false;
            showWebBtn.IsEnabled = false;

            if (!checkFileValidAndUpdateUI(filePath)) {
                interpCheckBox.IsEnabled = false;
                return;
            }

            interpCheckBox.IsEnabled = true;

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
            setVarNameLabel("未指定", "未指定", "未指定", "未指定");
        }

        private void checkIfIsWrfoutFile() {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir);
            var exceptionInfo = "";
            var isWrfoutFile = fileInfo.checkIsWrfoutFile(ref exceptionInfo);

            if (isWrfoutFile) {
                isNotWrfoutFile = false;
                varNameSelected = true;
                //timeNameTextBlock.
                setVarNameLabel("XTIME", "XLAT", "XLONG", "---");
                handleZLevelDim();

                selVarNameBtn.IsEnabled = false;
                startTrackingBtn.IsEnabled = true;
                zLvComboBox.IsEnabled = true;

            } else {
                isNotWrfoutFile = true;
                zDimLvCanSelect = false;
                zLvComboBox.SelectedIndex = -1;
                setVarNameLabel("未指定", "未指定", "未指定", "未指定");

                selVarNameBtn.IsEnabled = true;
            }
        }

        private void setVarNameLabel(string timeLabelName, string latLabelName, string lonLabelName, string varNameLabelName) {
            timeVarStr = timeLabelName;
            latVarStr = latLabelName;
            lonVarStr = lonLabelName;
            vorVarStr = varNameLabelName;
            
        }

        private void setVarName(string allStr) {
            
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
            //NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, s_TempFileDir);
            //var zLvDimName = "";
            //var zLvDimLen = fileInfo.getZLvDimLenName(ref zLvDimName);

            //if (zLvDimLen == 0) {    // 无z维度
            //    zLvNameTextBlock.Text = "(无)";
            //    zLvComboBox.SelectedIndex = -1;
            //    zDimLvCanSelect = false;
            //    return;
            //}
            //zLvNameTextBlock.Text = zLvDimName;
            //List<int> zLvIndex = Enumerable.Range(1, zLvDimLen).ToList();
            //zLvComboBox.ItemsSource = zLvIndex;
            //zLvComboBox.SelectedIndex = 0;
            //zDimLvCanSelect = true;
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
            if (e.PropertyName == "shouldInterp") {
                RaisePropertyChanged("interpTextBoxVisibility");
            }
        }
    }
}
