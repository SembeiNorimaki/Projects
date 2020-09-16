r1 = 51;
r2 = 14; //15
thick = 2;
$fn=100;
module a() {

  difference() {
    union() {
      rotate_extrude(convexity = 10) translate([r1-(r2/2), 0, 0]) 
        scale([1.0,2.0]) circle(r = r2/2);
      //button holder
      translate([0,r1-(r2/2)+17,0]) rotate([90,0,0]) 
        scale([1.0,1.0]) cylinder(h=17, r1=10, r2=r2);
      
      
    }
    //button hole
    translate([0,r1-(r2/2)+17,0]) rotate([90,0,0]) 
      scale([1.0,1.0]) cylinder(h=17, d=16); 
    
    // hollow structure
    color([1,0,0]) rotate_extrude(convexity = 10) translate([r1-(r2/2), 0, 0]) 
      scale([1.0,2.0]) circle(r = r2/2-thick);

    //translate([0,0,-50]) cube([100,100,100]);  //cross section cut
    translate([0,0,-50]) cylinder(h=2*r2+100,r = r1-(r2/2)); //vertical cut
    //translate([-8,-100,-4.5]) cube([16,100,10]); //charging hole
  }
  
  //holders for ground and glass
  difference() {
    union() {
      translate([0,0,r2-8]) cylinder(r=r1-(r2/2)+3.,h=6);
      //translate([0,0,-r2+3]) cylinder(r=r1-(r2/2)+3.5,h=2); //bottom
      color([1,0,0])translate([0,0,r2-8]) cylinder(r=r1-(r2/2)+4.5,h=2);
    }
    translate([0,0,-r2-10]) cylinder(r=r1-(r2/2)-3,h=100);
    translate([0,0,r2-6]) cylinder(r=r1-(r2/2),h=10);
    
  }

  //color([1,0,0])rotate([0,0,52.5]) translate([0, -r1+(r2/2), r2-5]) leds_holder();
  //rotate([0,0,-52.5]) translate([0, -r1+(r2/2), r2-5]) leds_holder();
  //rotate([0,0,180-52.5]) translate([0, -r1+(r2/2), r2-5]) leds_holder();
  //rotate([0,0,180+52.5]) translate([0, -r1+(r2/2), r2-5]) leds_holder();
  //rotate([0,0,300]) translate([0, -r1+(r2/2), r2-5]) leds_holder();
  //translate([r1-(r2/2)-3, -1.5, r2-2*thick]) cube([3,3,thick]);
  

// floor
difference() {
  translate([0,0,-r2]) cylinder(h=4, r=r1-(r2/2)+0);
  //cube([53,61,20],center=true);
}
//board
//color([0,0,1]) translate([0,0,r2-4-3.6]) cylinder(r=40,h=1.6);
//color([0,1,1]) translate([0,0,r2-4-3.6-9.5]) cylinder(r=40,h=9);


// battery
//color([1,0,1]) translate([0,-10,-5.6]) cube([60,40,10],center=true);
//color([1,0,1]) translate([0,0,-8]) cube([60,50,8.2],center=true);

//gps
//translate([-14,0,-r2+thick+10]) cube([27,38,10],center=true);
//arduino
//translate([14,0,-r2+thick+4]) cube([18,45,8],center=true);
//compass
//translate([14,0,-r2+thick+12]) cube([10,10,6],center=true);


//color([0,1,0]) translate([0,0,r2-5]) cylinder(h=thick, r=r1-(r2/2)-3);



}

module leds_holder() {
  translate([-1.5,0,0]) cube([3,4,thick]);
  translate([0,4,0]) cylinder(d=3,h=thick);
}

translate([0,0,0]) a();
//color([1,0,0])circle(d=86);
//leds_holder();

