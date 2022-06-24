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

using myCLI;

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for VarSelector.xaml
    /// </summary>
    public partial class VarSelector : Window {
        private string cSelDir {
            get => Properties.Settings.Default.selectDir;
        }
        public bool selectOK = false;

        public VarSelector() {
            InitializeComponent();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir);
            //fileInfo.openFile();
            var varsName = fileInfo.getVarsName();
            //aa.Clear();
            if (varsName.Count == 0) {
                MessageBox.Show("此文件无变量！请检查文件内容。");
                this.SourceInitialized += new EventHandler(MyForm_CloseOnStart);
                return;
            }
            //Debug.WriteLine(aa);
            comboBox_vor.ItemsSource = varsName;
            comboBox_vor.SelectedIndex = 0;
        }

        private void nextStepButton_Click(object sender, RoutedEventArgs e) {
            nextStepButton.IsEnabled = false;
            button_OK.IsEnabled = true;
            showComboBoxes();

            List<String> vorDimNames = new List<string>();
            NCFileInfo fileInfo = new NCFileInfo(cSelDir);
            fileInfo.getVorDimsName((string)comboBox_vor.SelectedValue, vorDimNames);

            comboBox_time.ItemsSource = vorDimNames;
            comboBox_lat.ItemsSource = vorDimNames;
            comboBox_lon.ItemsSource = vorDimNames;
            comboBox_time.SelectedIndex = 0;
            comboBox_lat.SelectedIndex = vorDimNames.Count - 2;
            comboBox_lon.SelectedIndex = vorDimNames.Count - 1;
        }

        private void showComboBoxes() {
            timeLabel.Visibility = Visibility.Visible;
            latLabel.Visibility = Visibility.Visible;
            lonLabel.Visibility = Visibility.Visible;
            comboBox_time.Visibility = Visibility.Visible;
            comboBox_lat.Visibility = Visibility.Visible;
            comboBox_lon.Visibility = Visibility.Visible;
        }

        private void MyForm_CloseOnStart(object sender, EventArgs e) {
            this.Close();
        }

        private void OKButton_Click(object sender, RoutedEventArgs e) {
            selectOK = true;
            this.Close();
        }

        private void Cancelbutton_Click(object sender, RoutedEventArgs e) {
            this.Close();
        }
    }
}
