using System;
using System.Collections.Generic;
using System.IO;
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
using MSHTML;

using Newtonsoft.Json;
using myCLI;
using System.ComponentModel;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for ResultWindow.xaml
    /// </summary>
    public partial class ResultWindow : Window, INotifyPropertyChanged {
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
        public List<Typhoon> tcsData;
        private List<List<Dictionary<string, float>>> tcsDataForJS = new List<List<Dictionary<string, float>>>();

        private HTMLDocumentEvents2_Event _docEvent;
        public event PropertyChangedEventHandler PropertyChanged;
        private void RaisePropertyChanged(string propertyName) {
            if (PropertyChanged != null) {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public ResultWindow() {
            InitializeComponent();
            //webBrowser.NavigateToString("<HTML><H2><B>This page comes using String</B><P></P></H2>");
            //Uri uri = new Uri(@"pack://application:,,,/data/myyy.html");
            //Stream source = Application.GetContentStream(uri).Stream;
            //webBrowser.NavigateToStream(source);
            
            webBrowser.NavigateToStream(System.Reflection.Assembly.GetEntryAssembly().GetManifestResourceStream("TC_Tracker.path.html"));
            //webBrowser.Document.inv
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


        private void exClick(object sender, RoutedEventArgs e) {
            Console.WriteLine("ex 1 tc!");
            int b = (int)webBrowser.InvokeScript("getTcIndex");
            Console.WriteLine("tcindex: {0}", b);
        }

        private void prevPageBtn_Click(object sender, RoutedEventArgs e) {
            --currentPage;
        }

        private void nextPageBtn_Click(object sender, RoutedEventArgs e) {
            ++currentPage;
        }
    }
}
