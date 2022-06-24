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

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

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
        //private List<Typhoon> realTCs = new List<Typhoon>();

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindow() {
            InitializeComponent();


            Console.WriteLine("sf");
            NCFileInfo e = new NCFileInfo("sdfaaa");
            String ee = e.echoFilePath();
            Console.WriteLine(ee);
        }

        private void browseButton_Click(object sender, RoutedEventArgs e) {
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
            selVarButton.IsEnabled = false;
            setVarNameLabel("未指定", "未指定", "未指定", "未指定");
        }

        private void checkIsWrfoutFile() {
            //    NCFileInfo fileInfo = new NCFileInfo(cSelDir, !isNotWrfoutFile, "", "", "", "", "");
            //    var exceptionInfo = "";
            //    var isWrfoutFile = fileInfo.checkIsWrfoutFile(ref exceptionInfo);

            //    if (isWrfoutFile) {
            //        isNotWrfoutFile = false;
            //        varNameSelected = true;
            //        //timeNameTextBlock.
            //        setVarNameLabel("XTIME", "XLAT", "XLONG", "---");
            //        handleZLevelDim();
            //    } else {
            //        isNotWrfoutFile = true;
            //        zDimLvCanSelect = false;
            //        zLvComboBox.SelectedIndex = -1;
            //        setVarNameLabel("未指定", "未指定", "未指定", "未指定");
            //    }
        }

        private void setVarNameLabel(string timeLabelName, string latLabelName, string lonLabelName, string varNameLabelName) {
            timeNameTextBlock.Text = timeLabelName;
            latNameTextBlock.Text = latLabelName;
            lonNameTextBlock.Text = lonLabelName;
            vorNameTextBlock.Text = varNameLabelName;
        }
    }
}
