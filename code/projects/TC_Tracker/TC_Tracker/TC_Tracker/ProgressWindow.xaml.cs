using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;
using myCLI;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for ProgressWindow.xaml
    /// </summary>
    public partial class ProgressWindow : Window {

        private NCFileInfo tracker;
        private List<Typhoon> realTCs = new List<Typhoon>();

        private System.Threading.CancellationTokenSource m_Cts;
        private System.Threading.CancellationToken m_Ct;

        public ProgressWindow() {
            InitializeComponent();
        }

        public ProgressWindow(NCFileInfo tracker): this() {
            this.tracker = tracker;
            //var process = new Progress<string>((p) => {
            //    label.Content = label.Content + "!";
            //});
            asyncRun();
            
        }

        private async void asyncRun() {
            m_Cts = new System.Threading.CancellationTokenSource();
            m_Ct = m_Cts.Token;

            await Task.Run(() => {
                tracker.startTracking(realTCs, stepPgCallBack, progressCallBack, m_Ct);

                Dispatcher.BeginInvoke(DispatcherPriority.Input, new Action(() => {
                    MainWindow mainW = (MainWindow)Owner;

                    if (m_Ct.IsCancellationRequested) {
                        levelLabel.Content = "已取消";
                        mainW.canceledLabel.Visibility = Visibility.Visible;
                        Close();
                    } else {
                        mainW.setRealTCs(realTCs);

                        mainW.showWebBtn.IsEnabled = true;
                        Close();
                    }
                }));
            });
        }

        private void stepPgCallBack(int stepIdx) {
            var labelStr = "";
            switch (stepIdx) {
            case 0:
                labelStr = "读取原始数据...";
                break;
            case 1:
                labelStr = "regridding...";
                break;
            case 2:
                labelStr = "计算涡度中...";
                break;
            case 3:
                labelStr = "获取原始涡旋...";
                break;
            case 4:
                labelStr = "连接涡旋...";
                break;
            case 5:
                labelStr = "去除噪声...";
                break;
            default:
                labelStr = "";
                break;
            }
            Dispatcher.BeginInvoke(DispatcherPriority.Input, new Action(() => {
                levelIndicator.Value = stepIdx * 20;
                levelLabel.Content = labelStr;
            }));
        }

        private void progressCallBack(double progressValue) {
            Dispatcher.BeginInvoke(DispatcherPriority.Input, new Action(() => {
                if (progressValue >= 0) {
                    if (progressBar.IsIndeterminate) {
                        progressBar.IsIndeterminate = false;
                    }
                    progressBar.Value = progressValue;
                } else if (!progressBar.IsIndeterminate) {
                    progressBar.IsIndeterminate = true;
                }
            }));
        }

        private void cancelBtnClicked(object sender, RoutedEventArgs e) {
            cancelBtn.IsEnabled = false;
            progressBar.IsIndeterminate = true;
            levelLabel.Content = "取消中...";
            levelIndicator.Value = levelIndicator.Maximum;
            levelIndicator.Foreground = Brushes.Red;

            m_Cts.Cancel();
        }
    }
}
