//
//  ProgressViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/7/7.
//

import Cocoa

class ProgressViewController: NSViewController {
    
    @IBOutlet var progressBar: NSProgressIndicator!
    
    
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
            let realTCs = tracker.startTracking(stepPgCallback: { (stepIdx, observer) in
                
            }, andWith: { (progressValue, observer) in
                let mySelf = Unmanaged<ProgressViewController>.fromOpaque(observer!).takeUnretainedValue()
                DispatchQueue.main.async {
//                    let oldValue = mySelf.progress.stringValue
//                    mySelf.progress.stringValue = oldValue + "."
                    if (mySelf.progressBar.isIndeterminate) {
                        mySelf.progressBar.isIndeterminate = false
                    }
                    mySelf.progressBar.doubleValue = progressValue
                }
            }, withTarget: observer).compactMap { $0 as? Typhoon }
            
            mainVC.setRealTCs(tcs: realTCs)
            
            DispatchQueue.main.async {
                mainVC.showWebBtn.isEnabled = true
                mainVC.dismiss(self)
            }
        }
    }
    
}
