//
//  ProgressViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/7/7.
//

import Cocoa

class ProgressViewController: NSViewController {
    
    @IBOutlet var progressBar: NSProgressIndicator!
    @IBOutlet var levelIndicator: NSLevelIndicator!
    @IBOutlet var levelLabel: NSTextField!
    
    
    public var tracker: NCFileInfo_Wrapper?
    private var realTCs: [Typhoon] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        
        progressBar.startAnimation(nil)
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        
        guard let tracker = tracker else { return }
        guard let mainVC = presentingViewController as? ViewController else { return }
        
        let dispatchQueue = DispatchQueue(label: "QueueIdentification", qos: .userInitiated)
        let observer = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        dispatchQueue.async {
            let tcs = tracker.startTracking(stepPgCallback: { (stepIdx, observer) in
                let mySelf = Unmanaged<ProgressViewController>.fromOpaque(observer!).takeUnretainedValue()
                DispatchQueue.main.async {
                    mySelf.levelIndicator.intValue = stepIdx
                    let labelStr: String
                    switch stepIdx {
                    case 0:
                        labelStr = "读取原始数据..."
                    case 1:
                        labelStr = "regridding..."
                    case 2:
                        labelStr = "计算涡度中..."
                    case 3:
                        labelStr = "获取原始涡旋..."
                    case 4:
                        labelStr = "连接涡旋..."
                    case 5:
                        labelStr = "去除噪声..."
                    default:
                        labelStr = ""
                    }
                    mySelf.levelLabel.stringValue = labelStr
                }
            }, andWith: { (progressValue, observer) in
                let mySelf = Unmanaged<ProgressViewController>.fromOpaque(observer!).takeUnretainedValue()
                DispatchQueue.main.async {

                    if progressValue >= 0 {
                        if mySelf.progressBar.isIndeterminate {
                            mySelf.progressBar.isIndeterminate = false
                        }
                        mySelf.progressBar.doubleValue = progressValue
                    } else if !mySelf.progressBar.isIndeterminate {
                        mySelf.progressBar.isIndeterminate = true
                        mySelf.progressBar.startAnimation(nil)
                    }
                    
                }
            }, withTarget: observer)!//.tcs.compactMap { $0 as? Typhoon }
            
            mainVC.setTCs(tcs: tcs)
            
            DispatchQueue.main.async {
                mainVC.showWebBtn.isEnabled = true
                mainVC.dismiss(self)
            }
        }
    }
    
}
