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
        public MainWindow() {
            InitializeComponent();


            Console.WriteLine("sf");
            NCFileInfo e = new NCFileInfo("sdfaaa");
            String ee = e.echoFilePath();
            Console.WriteLine(ee);
        }

        private void browseButton_Click(object sender, RoutedEventArgs e) {
            Console.WriteLine("browse button clicked!");

            //var dialog = new CommonOpenFileDialog();

            //var savedSDir = cSelDir;
            //dialog.InitialDirectory = string.IsNullOrEmpty(savedSDir) ? "C:\\Users" : savedSDir;
            //dialog.RestoreDirectory = true;
            ////dialog.IsFolderPicker = true;
            //if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
            //    trackFinished = false;
            //    var filePath = dialog.FileName;
            //    Debug.WriteLine(filePath);

            //    ncFileTextBox.Text = filePath;
            //    //changeUIAccV(validateDir(dirTextBox.Text));
            //    if (!checkFileValidAndUpdateUI(filePath)) {
            //        selectedFile = false;
            //        return;
            //    }
            //    cSelDir = filePath;
            //    selectedFile = true;
            //    checkIsWrfoutFile();
            //}
        }
    }
}
