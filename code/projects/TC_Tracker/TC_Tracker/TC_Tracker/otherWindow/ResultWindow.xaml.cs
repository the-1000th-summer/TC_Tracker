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
using System.ComponentModel;
using mshtml;

using Newtonsoft.Json;
using myCLI;

namespace TC_Tracker.otherWindow {
    /// <summary>
    /// Interaction logic for ResultWindow.xaml
    /// </summary>
    public partial class ResultWindow : Window {

        private int _pages = 1;
        public int pages {
            get { return _pages; }
            set {
                _pages = value;
                RaisePropertyChanged("pages");
            }
        }
        private int _currentPage = 1;
        public int currentPage {
            get { return _currentPage; }
            set {
                _currentPage = value;
                draw(_currentPage);
                RaisePropertyChanged("currentPage");
            }
        }

        private HTMLDocumentEvents2_Event _docEvent;
        public event PropertyChangedEventHandler PropertyChanged;

        private List<Typhoon> tcsData;
        private List<List<Dictionary<string, float>>> tcsDataForJS = new List<List<Dictionary<string, float>>>();

        public ResultWindow() {
            InitializeComponent();
        }

        public ResultWindow(List<Typhoon> tcsData): this() {
            this.tcsData = tcsData;

            webBrowser.NavigateToStream(System.Reflection.Assembly.GetEntryAssembly().GetManifestResourceStream("TC_Tracker.path.html"));
        }

        private void loadCompletedHandler(object sender, System.Windows.Navigation.NavigationEventArgs e) {
            prepareData();
            webBrowser.InvokeScript("drawBaseMap");
            draw(1);
            if (_docEvent != null) {
                _docEvent.oncontextmenu -= _docEvent_oncontextmenu;
            }
            if (webBrowser.Document != null) {
                _docEvent = (HTMLDocumentEvents2_Event)webBrowser.Document;
                _docEvent.oncontextmenu += _docEvent_oncontextmenu;
            }
        }

        bool _docEvent_oncontextmenu(IHTMLEventObj pEvtObj) {
            WbShowContextMenu();
            return false;
        }

        public void WbShowContextMenu() {
            ContextMenu cm = FindResource("MnuCustom") as ContextMenu;
            if (cm == null)
                return;
            cm.PlacementTarget = webBrowser;
            cm.IsOpen = true;
        }

        private void prepareData() {

            foreach (var tcData in tcsData) {
                var tcDataForJS = new List<Dictionary<string, float>>();
                foreach (var i in tcData.geoCenters) {
                    var tempDict = new Dictionary<string, float>();
                    tempDict.Add("lat", i.Item1);
                    tempDict.Add("lon", i.Item2);
                    tcDataForJS.Add(tempDict);
                }
                tcsDataForJS.Add(tcDataForJS);
            }
            pages = tcsDataForJS.Count / 10 + 1;
            //pagesLabel.Content = "1/" + pages.ToString();
        }

        private void draw(int currentPage) {
            try {
                int startIndex = (currentPage - 1) * 10;
                int countIn1Page = (currentPage == pages) ? (tcsDataForJS.Count - startIndex) : 10;
                var serializedStr = JsonConvert.SerializeObject(tcsDataForJS.GetRange(startIndex, countIn1Page));
                //Console.WriteLine(bbba);
                webBrowser.InvokeScript("delPath");
                webBrowser.InvokeScript("drawPath", serializedStr);
            } catch (Exception ex) {
                string msg = "Could not call script: " + ex.Message + "\n\nPlease click the 'Load HTML Document with Script' button to load.";
                MessageBox.Show(msg);
            }
        }

        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        protected override void OnLocationChanged(EventArgs e) {
            myPopup.HorizontalOffset += 1;
            myPopup.HorizontalOffset -= 1;
            base.OnLocationChanged(e);
        }

        protected override void OnDeactivated(EventArgs e) {
            base.OnDeactivated(e);
            myPopup.IsOpen = false;
        }

        protected override void OnActivated(EventArgs e) {
            base.OnActivated(e);
            myPopup.IsOpen = true;
        }
    }
}
