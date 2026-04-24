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

using myCLI;

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for VarSelectPage1.xaml
    /// </summary>
    public partial class VarSelectPage1 : Page, INotifyPropertyChanged {

        private string cSelDir {
            get => Properties.Settings.Default.selectDir;
            set {
                Properties.Settings.Default.selectDir = value;
                Properties.Settings.Default.Save();
            }
        }

        private bool _vorNameIsEnabled = false;
        public bool vorNameIsEnabled {
            get => _vorNameIsEnabled;
            set {
                _vorNameIsEnabled = value;
                RaisePropertyChanged(nameof(vorNameIsEnabled));
            }
        }

        private bool _windNameIsEnabled = false;
        public bool windNameIsEnabled {
            get => _windNameIsEnabled;
            set {
                _vorNameIsEnabled = value;
                RaisePropertyChanged(nameof(windNameIsEnabled));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public VarSelectPage1() {
            InitializeComponent();


            
        }

        private void pageLoaded(object sender, RoutedEventArgs e) {
            NCFileInfo fileInfo = new NCFileInfo(cSelDir);
            var varsName = fileInfo.getVarsName();
            if (varsName.Count == 0) {
                MessageBox.Show("此文件无变量！请检查文件内容。");
                //this.SourceInitialized += new EventHandler(MyForm_CloseOnStart);
                Window varSelWindow = (Window)(this.Parent);
                varSelWindow.Close();
                return;
            }
            vorNameComboBox.ItemsSource = varsName;
            uwndNameComboBox.ItemsSource = varsName;
            vwndNamecomboBox.ItemsSource = varsName;
        }

        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void nextStepBtnClicked(object sender, RoutedEventArgs e) {
            if (!checkComboBoxes()) { return; }

            var vorOrWindNames = (vorRadioButton.IsChecked ?? false) ? new List<string> { vorNameComboBox.SelectedItem.ToString() } : new List<string> { uwndNameComboBox.SelectedItem.ToString(), vwndNamecomboBox.SelectedItem.ToString() };
            
            var secondPage = new VarSelectPage2(vorOrWindNames);
            NavigationService.Navigate(secondPage);
        }

        private bool checkComboBoxes() {
            List<ComboBox> comboBoxes = (vorRadioButton.IsChecked ?? false) ? new List<ComboBox> { vorNameComboBox } : new List<ComboBox> { uwndNameComboBox, vwndNamecomboBox };

            foreach (var comboBox in comboBoxes) {
                var selectedIndex = comboBox.SelectedIndex;
                if (selectedIndex == -1) {
                    MessageBox.Show("变量名不能为空！");
                    return false;
                }
            }
            return true;
        }

        private void cancelBtnClicked(object sender, RoutedEventArgs e) {
            Window varSelWindow = (Window)(this.Parent);
            varSelWindow.Close();
        }
    }
}
