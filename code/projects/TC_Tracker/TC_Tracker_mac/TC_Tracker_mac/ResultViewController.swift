//
//  ResultViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/7/3.
//

import Cocoa
import WebKit

class ResultViewController: NSViewController {

    @IBOutlet var webView: WKWebView!
    
    public var tcsData: [Typhoon] = []
    private var tcsDataForJS: [[[String: Float]]] = []
    private var pages: Int = 1
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        webView.navigationDelegate = self
        
        if let path = Bundle.main.url(forResource: "path", withExtension: "html"){
            let myURLRequest:URLRequest = URLRequest(url: path)
            webView.load(myURLRequest)
        }
    }
    
    override func viewWillAppear() {
        super.viewWillAppear()
        prepareData()
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        
        print("did appert")
//        webView.evaluateJavaScript("changeBackgroundColor('red')") { (result, error) in
//            print(error)
//        }
    }
    
    private func prepareData() {
        tcsDataForJS = tcsData.map {
            $0.geoCenters.map {
                ["lat": ($0 as! LatLon).lat, "lon": ($0 as! LatLon).lon]
            }
        }
        pages = tcsDataForJS.count / 10 + 1
    }
    
    private func draw(currentPage: Int) {
        let startIndex = (currentPage - 1) * 10
        let tcNumIn1Page = (currentPage == pages) ? (tcsDataForJS.count - startIndex) : 10
        
        let jsonEncoder = JSONEncoder()
        let jsonData = try! jsonEncoder.encode(Array(tcsDataForJS[startIndex..<(startIndex+tcNumIn1Page)]))
        let json = String(data: jsonData, encoding: String.Encoding.utf8)!
        
        webView.evaluateJavaScript("delPath()") { (result, error) in
            if let error = error {
                self.raiseAlert(msgText: "Javascript Error", infoText: error.localizedDescription)
            }
        }
        webView.evaluateJavaScript("drawPath('\(json)')") { (result, error) in
            if let error = error {
                self.raiseAlert(msgText: "Javascript Error", infoText: error.localizedDescription)
            }
        }
    }
    
    private func raiseAlert(msgText: String, infoText: String) {
        let alert = NSAlert()
        alert.messageText = msgText
        alert.informativeText = infoText
        alert.runModal()
    }
    
}

extension ResultViewController: WKNavigationDelegate {

    func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
        print("Finished navigating to url \(webView.url)")

        webView.evaluateJavaScript("drawBaseMap()", completionHandler: nil)
        draw(currentPage: 1)
    }
}
