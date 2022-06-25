using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Globalization;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace TC_Tracker {
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application {
    }

    [ValueConversion(typeof(bool), typeof(bool))]
    public class InverseBooleanConverter : IValueConverter {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter,
            System.Globalization.CultureInfo culture) {
            if (targetType == typeof(bool)) {
                return !(bool)value;
            } else if (targetType == typeof(bool?)) {
                return !(bool?)value ?? true;
            }
            throw new InvalidOperationException("The target must be a boolean. target type: " + targetType.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter,
            System.Globalization.CultureInfo culture) {
            throw new NotSupportedException();
        }

        #endregion
    }

    public class PrevPageBtnConverter : IValueConverter {
        public object Convert(object value, Type targetType, object parameter,
            System.Globalization.CultureInfo culture) {
            //if (targetType != typeof(int))
            //    throw new InvalidOperationException("The target must be a int. target type: " + targetType.ToString());
            return ((int)value == 1) ? false : true;
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            throw new NotSupportedException();
        }
    }

    public class StartTrackLabelConverter : IValueConverter {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            if (!(value is bool))
                throw new InvalidOperationException("The target must be a boolean. target type: " + value.GetType().ToString());
            return (bool)value ? "开始识别" : "识别中...";
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            throw new NotSupportedException();
        }
    }
    public class StopTrackLabelConverter : IMultiValueConverter {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture) {
            if (!(values[0] is bool) && !(values[1] is bool))
                throw new InvalidOperationException("The target must be a boolean. target type: " + values[0].GetType().ToString());
            if ((bool)values[0])
                return "中止";
            if (!(bool)values[1])
                return "正在中止...";
            return "中止";
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) {
            throw new NotSupportedException();
        }
    }

    public class NextPageBtnConverter : IMultiValueConverter {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture) {
            return ((int)values[0] == (int)values[1]) ? false : true;
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) {
            throw new NotSupportedException();
        }
    }

    public class BooleanAndConverter : IMultiValueConverter {
        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture) {
            foreach (object value in values) {
                if ((value is bool) && (bool)value == false) {
                    return false;
                }
            }
            return true;
        }
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture) {
            throw new NotSupportedException("BooleanAndConverter is a OneWay converter.");
        }
    }
}
