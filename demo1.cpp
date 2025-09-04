#include <iostream>
#include "TF1.h"
#include "TCanvas.h"
int main(int argc,char **argv)
{
        TCanvas* c=new TCanvas("c","first root",0,0,800,600);
        TF1 *f1=new TF1("f1","sin(x)",-5,5);
        f1->SetLineColor(kBlue+2);
        f1->SetTitle("MY graph;x;sin(x)");
        f1->Draw();
        c->Print("demo.pdf");
        return 0;
}
