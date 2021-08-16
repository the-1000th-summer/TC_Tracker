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
using System.Diagnostics;

using myCLI;

namespace TC_Tracker {
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
            NCFileInfo fileInfo = new NCFileInfo(cSelDir,false,"","","","");
            //fileInfo.openFile();
            var aa = fileInfo.getVarsName();
            //aa.Clear();
            if (aa.Count == 0) {
                MessageBox.Show("此文件无变量！请检查文件内容。");
                this.SourceInitialized += new EventHandler(MyForm_CloseOnStart);
                return;
            }
            //Debug.WriteLine(aa);
            comboBox_lat.ItemsSource = aa;
            comboBox_lon.ItemsSource = aa;
            comboBox_vor.ItemsSource = aa;
            comboBox_lat.SelectedIndex = 0;
            comboBox_lon.SelectedIndex = 0;
            comboBox_vor.SelectedIndex = 0;
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
