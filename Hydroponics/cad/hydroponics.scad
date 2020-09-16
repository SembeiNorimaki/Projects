$fn=100;
module pot() {
    difference() {
        cylinder(d=340,h=320);
        translate([0,0,5]) cylinder(d=330,h=320);
    }
}
module vert_tube() {
    difference() {
        translate([0,0,5]) cylinder(d=110.2+1,h=2000);
        translate([0,0,5]) cylinder(d=110.2-6,h=2010);
    }
}
module top() {
    dExt = 323.5;
    dIn = 111;
    
    difference() {
        circle(d=dExt);
        circle(d=dIn);
        translate([-150,0,0]) circle(r=150,$fn=3);  //triangle
    }
    
}
module tapa_top() {
        dExt = 323.5;
    dIn = 111;
    difference() {
        translate([-150,0,0]) circle(r=150,$fn=3);  //triangle
        difference() {
            circle(d=dExt*2);
            circle(d=dExt);
        }
        circle(d=dIn);
        translate([-162,0,0]) circle(d=20);
        
}
}


module middle() {
    dExt = 340;
    dIn = 110;
    difference() {
        circle(d=dExt);
        circle(d=dIn);
    }
}
module holder() {
    
    //color([0,1,0]) translate([-110/2+1,0,0]) cylinder(d=320,h=2);
    a=30;
    //vert_tube();
    //translate([15-4,0,22]) rotate([0,90,0]) cylinder(d=7.2,h=4);
    difference() {
        translate([0,-a/2,0]) cube([a,a,a]);
        translate([15,-a/2-1,6+5]) cube([a,a+2,a]);
        translate([15+5,-a/2-1,6]) cube([a,a+2,a]);
        translate([15+5,0,6+5]) rotate([90,0,0]) cylinder(r=5,h=50,center=true);
        translate([-110/2+8,0,0]) vert_tube();
        translate([-2,0,22]) rotate([0,90,0]) cylinder(d=4.4,h=300); //screw hole
        translate([15-4,0,22]) rotate([0,90,0]) cylinder(d=7.5,h=4+1); //screw head hole
        translate([-1,0,22]) rotate([0,90,0]) cylinder(d=5,h=3.2+1); //nut hole
        //translate([0,-50,22]) cube([100,100,100]);
        
    }
}

module holder_top() {
    difference() {
    translate([325/2-4.8,-10,0]) cube([5,20,5]);
        difference() {
    
    cylinder(d=400,h=100);
    translate([0,0,-1])cylinder(d=325,h=102);
    }
}
}
//holder_top();
//top();
//tapa_top();
//middle();
holder();
//pot();
//vert_tube();
//rotate([0,0,0*360/6]) translate([0,0,5]) color([1,0,0]) holder();
//rotate([0,0,1*360/6]) translate([0,0,5]) color([1,0,0]) holder();
//rotate([0,0,2*360/6]) translate([0,0,5]) color([1,0,0]) holder();
//rotate([0,0,3*360/6]) translate([0,0,5]) color([1,0,0]) holder();
//rotate([0,0,4*360/6]) translate([0,0,5]) color([1,0,0]) holder();
//rotate([0,0,5*360/6]) translate([0,0,5]) color([1,0,0]) holder();