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

using myCLI;

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for VarSelectPage2.xaml
    /// </summary>
    public partial class VarSelectPage2 : Page {

        private string cSelDir {
            get => Properties.Settings.Default.selectDir;
            set {
                Properties.Settings.Default.selectDir = value;
                Properties.Settings.Default.Save();
            }
        }

        private List<string> vorOrWindNames;
        private List<string> dimsName;

        public VarSelectPage2() {
            InitializeComponent();
        }

        public VarSelectPage2(List<string> vorOrWindNames) : this() {
            this.vorOrWindNames = vorOrWindNames;

            dimsName = (new NCFileInfo(cSelDir)).getVorDimsName(vorOrWindNames[0]);

            timeComboBox.ItemsSource = dimsName;
            latComboBox.ItemsSource = dimsName;
            lonComboBox.ItemsSource = dimsName;

            timeComboBox.SelectedIndex = 0;
            latComboBox.SelectedIndex = dimsName.Count - 2;
            lonComboBox.SelectedIndex = dimsName.Count - 1;
        }

        private void cancelBtnClicked(object sender, RoutedEventArgs e) {
            Window varSelWindow = (Window)(this.Parent);
            varSelWindow.Close();

        }

        private void okBtnClicked(object sender, RoutedEventArgs e) {
            if (!checkComboBoxes()) { return; }

            var vorName = (vorOrWindNames.Count == 1) ? vorOrWindNames[0] : "";
            var uwndName = (vorOrWindNames.Count == 1) ? "" : vorOrWindNames[0];
            var vwndName = (vorOrWindNames.Count == 1) ? "" : vorOrWindNames[1];

            VarSelectWindow varSelWindow = (VarSelectWindow)(this.Parent);
            varSelWindow.getVarNames(new List<string> { timeComboBox.SelectedItem.ToString(), latComboBox.SelectedItem.ToString(), lonComboBox.SelectedItem.ToString(), vorName, uwndName, vwndName });

            varSelWindow.Close();
        }

        private bool checkComboBoxes() {
            var comboBoxes = new List<ComboBox> { timeComboBox, latComboBox, lonComboBox };

            foreach (var comboBox in comboBoxes) {
                var selectedIndex = comboBox.SelectedIndex;
                if (selectedIndex == -1) {
                    MessageBox.Show("变量名不能为空！");
                    return false;
                }
            }
            return true;
        }
    }
}
