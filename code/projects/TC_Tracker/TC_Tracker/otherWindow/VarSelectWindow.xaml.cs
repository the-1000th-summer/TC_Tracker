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
using System.Windows.Navigation;

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for VarSelectWindow.xaml
    /// </summary>
    public partial class VarSelectWindow : NavigationWindow {
        public delegate void SendName(List<string> name);
        public event SendName onVarNamesSend;

        public VarSelectWindow() {
            InitializeComponent();
            ShowsNavigationUI = false;
            
        }

        public void getVarNames(List<string> allVarNames) {
            onVarNamesSend(allVarNames);
        }
    }
}
