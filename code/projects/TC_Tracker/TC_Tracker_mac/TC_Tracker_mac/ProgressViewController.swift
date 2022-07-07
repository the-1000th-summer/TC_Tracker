//
//  ProgressViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/7/7.
//

import Cocoa

class ProgressViewController: NSViewController {
    
    public var tracker: NCFileInfo_Wrapper?
    private var realTCs: [Typhoon] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        
        
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        
        guard let tracker = tracker else { return }
        guard let mainVC = presentingViewController as? ViewController else { return }
        
        let dispatchQueue = DispatchQueue(label: "QueueIdentification", qos: .userInitiated)
        let observer = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        dispatchQueue.async {
            let realTCs = tracker.startTracking(callback: { observer in
                print("swift: one progress!")
            }, andWith: { (result: Bool, observer) in
                
            }, withTarget: observer).compactMap { $0 as? Typhoon }
            
            mainVC.setRealTCs(tcs: realTCs)
        }
    }
    
}
