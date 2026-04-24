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

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for SettingsWindow.xaml
    /// </summary>
    public partial class SettingsWindow : Window {
        public SettingsWindow() {
            InitializeComponent();
            setThreadInfo();
        }

        private void comboboxSelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cmb = sender as ComboBox;
            Properties.Settings.Default.threadNum = cmb.SelectedIndex + 1;
            Properties.Settings.Default.Save();
        }
        

        private void setThreadInfo() {
            int coreCount = getCoreCount();
            List<int> comboboxList = Enumerable.Range(1, coreCount).ToList();
            coreCountComboBox.ItemsSource = comboboxList;

            coreCountComboBox.SelectedIndex = Properties.Settings.Default.threadNum - 1;
        }

        private int getCoreCount() {
            int coreCount = 0;
            foreach (var item in new System.Management.ManagementObjectSearcher("Select NumberOfCores from Win32_Processor").Get()) {
                coreCount += int.Parse(item["NumberOfCores"].ToString());
            }
            return coreCount;
        }
    }
}
