using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
    /// Interaction logic for ParaSelWindow.xaml
    /// </summary>
    public partial class ParaSelWindow : Window {
        private float htmr1e5 {
            get => Properties.Settings.Default.HTMR;
            set {
                Properties.Settings.Default.HTMR = value;
                Properties.Settings.Default.Save();
            }
        }

        public ParaSelWindow() {
            InitializeComponent();
            HTMRTextBox.Text = htmr1e5.ToString("0.0000");
        }

        private void NumberValidationTextBox(object sender, TextCompositionEventArgs e) {
            Regex regex = new Regex("[^0-9.]+");
            e.Handled = regex.IsMatch(e.Text);
        }

        private void checkIfTextBoxValid() {
            try {
                htmr1e5 = (float)Convert.ToDouble(HTMRTextBox.Text);
            } catch (System.FormatException) {
                MessageBox.Show("所填数字无法解析: " + HTMRTextBox.Text);
                return;
            }
            this.Close();
        }

        private void OKButton_Click(object sender, RoutedEventArgs e) {
            checkIfTextBoxValid();
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e) {
            this.Close();
        }
    }
}
