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
using Microsoft.WindowsAPICodePack.Dialogs;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for SettingsWindow.xaml
    /// </summary>
    public partial class SettingsWindow : Window {

        private string s_TempFileDir {
            get => Properties.Settings.Default.tempFileDir;
            set {
                Properties.Settings.Default.tempFileDir = value;
                Properties.Settings.Default.Save();
            }
        }
        //private string tempFileDir_temp = "";

        public SettingsWindow() {
            InitializeComponent();
            tempFDirTextBox.Text = s_TempFileDir;
        }

        private void browsebutton_Click(object sender, RoutedEventArgs e) {
            var dialog = new CommonOpenFileDialog();

            var savedSDir = s_TempFileDir;
            dialog.InitialDirectory = savedSDir == "" ? "C:\\Users" : savedSDir;
            dialog.IsFolderPicker = true;
            //dialog. = this;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
                var selectDir = dialog.FileName;
                //s_TempFileDir = selectDir;
                //tempFileDir_temp = selectDir;
                tempFDirTextBox.Text = selectDir;
            }
        }

        /// <summary>
        /// 文件夹textbox有焦点，并按下enter键，全选textbox并保存路径
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DirTextBox_OnKeyDown(object sender, KeyEventArgs e) {
            if (e.Key == Key.Enter) {
                tempFDirTextBox.SelectAll();
                
                //changeUIAccV(validateDir(tempFDirTextBox.Text));
            }
        }

        private void DirTextBox_OnLostFocus(object sender, RoutedEventArgs e) {
            Console.WriteLine("asdsfd");
        }

        

        private void OKbutton_Click(object sender, RoutedEventArgs e) {
            var dirIsValid = checkDirIsValid();
            if (!dirIsValid) {
                MessageBox.Show("临时文件夹不合法！");
                return;
            }
            //saveSelectDir(tempFDirTextBox.Text);
            s_TempFileDir = tempFDirTextBox.Text;
            this.Close();
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e) {
            this.Close();
        }

        private bool checkDirIsValid() {
            return true;
        }


        /// <summary>
        /// 保存工作文件夹字符串到settings中
        /// </summary>
        /// <param name="text"></param>
        private void saveSelectDir(string text) {
            Properties.Settings.Default.selectDir = text;
            Properties.Settings.Default.Save();
        }

        
    }
}
