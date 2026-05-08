//
//  PreferencesViewController.swift
//  TC_Tracker_mac
//
//  Created on 2022/7/29.
//

import Cocoa

class PreferencesViewController: NSViewController {

    @IBOutlet var stepper: NSStepper!
    
    @objc private dynamic var threadNum = 1 {
        didSet {
            UserDefaults.standard.set(threadNum, forKey: "ThreadNum")
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        setThreadInfo()
    }
    
    private func setThreadInfo() {
        stepper.maxValue = Double(ProcessInfo().activeProcessorCount)
        if UserDefaults.standard.object(forKey: "ThreadNum") == nil {
            threadNum = 1
            return
        }
        threadNum = UserDefaults.standard.integer(forKey: "ThreadNum")
    }
    
}
