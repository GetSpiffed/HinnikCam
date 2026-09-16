// LilyGO T-Camera S3 enclosure with 34 x 56 x 5.2 mm LiPo.
// Based on espcam_TCameraS3_voorlopig.scad; PCB and component positions unchanged.
// Housing widened; rear cover deepened; battery supports added.
// Battery wire exits the middle of the +Y short side (OLED end).
// Use thin soft pads / nonconductive retention; do not clamp the pouch.
// Rear mount screw/nut must stay within the 3 mm reserved space.
// Original USB opening retained. Check actual cable fit.
// TYPE: preview, front, back, backmount, wallmount.
// Preview is an exploded view, not the assembled enclosure.
// Geometry inspected numerically; OpenSCAD render / physical fit not tested.

$fs=0.5;
$fa=6;

//animation view parameters
/*
$vpt = [0, 0, -28]; 

ROT = 90;
r = $t < 0.5 ? $t*4*ROT : 2*ROT- ($t-0.5)*ROT *4;
 
$vpr = [75,0,250-90+r];
//$vpr = [75,0,250];
$vpd = 350;
*/

// Type of model 
TYPE = "front"; // [preview, front, back, backmount, wallmount]

//M3 hole
M3_HOLE_DIA = 3.8;
//M3 hole for tapping
M3_TAP_DIA = 2.8;
//M3 nut diameter
M3_NUT_DIA = 6.01;
//M3 nut height (thickness)
M3_NUT_HEIGHT = 2.4;
//M3 screw head diameter
M3_HEAD_DIA = 6.5;

// Diameter of PCB mounting holes
PCB_MOUNT_HOLE_DIA = 3.5;
// Diameter of PCB rounded edges (has same center as PCB mounting holes)
PCB_EDGE_DIA = 4.5;
// PCB width
PCB_X = 28.0;
// PCB height
PCB_Y = 69.0;
// PCB thickness
PCB_Z = 1.2;

// LiPo dimensions supplied by user; PCB dimensions remain unchanged.
BATTERY_X = 34;
BATTERY_Y = 56;
BATTERY_Z = 5.2;
BATTERY_CLEARANCE_XY = 0.5;
BATTERY_CLEARANCE_Z = 0.4;
// Free zone between battery and rear wall for mount hardware.
BATTERY_MOUNT_CLEARANCE = 3;
// Cable exits centrally at +Y, the short side facing the OLED end.
BATTERY_WIRE_WIDTH = 6;
BATTERY_WIRE_RUN = 4;
BATTERY_WIRE_HEIGHT = 3;
CASE_X = max(PCB_X, BATTERY_X + 2*BATTERY_CLEARANCE_XY);

// OLED width
OLED_X = 25;
// OLED total height
OLED_Y = 16.8;
// OLED height of real display area
OLED_Y_VISIBLE = 14;
// OLED Z coordinate of surface
OLED_Z = 3.7;
// Distance of OLED from PCB top
OLED_TOP_OFFSET = 11.6;

// Diameter of camera lens (at top surface)
LENS_DIA = 7.5 + 0.5;
// Distance of camera lens center from PCB bottom
LENS_BOTTOM_OFFSET = 27.5;
// Z coordinate of camera lens surface
LENS_Z = 10;

// Diameter of PIR sensor cap
PIR_DIA = 12.5+0.5;
// Distance of PIR sensor cap from PCB bottom
PIR_BOTTOM_OFFSET = 14;
// Z coordinate of PIR sensor cap base (lowest Z of removable cap)
PIR_BASE_Z = 5.0;

// Width/height of buttons
BUTTON_XY = 5+0.5;
// Diameter of buttons rounded edges
BUTTON_EDGE_DIA = 1.5;
// Distance of buttons from PCB bottom (should be the aligned with the PIR sensor)
BUTTON_BOTTOM_OFFSET = PIR_BOTTOM_OFFSET - (BUTTON_XY/2);
// Distance of buttons from PCB side
BUTTON_SIDE_OFFSET = 1.0; 
// Z coordinate of button surface
BUTTON_Z = 5.0;

// Width of micro USB socket
USB_Y = 10;
// Height of micro USB socket
USB_X = 7.5;
// Z coordinate of micro USB socket surface 
USB_Z = 2.45 - PCB_Z;
// Distance of USB socket from PCB bottom
USB_BOTTOM_OFFSET = -1.5;

// Wall thickness
WALL_THICKNESS = 2.0;
// Use a thinner wall on front
FRONT_WALL_THICKNESS = PCB_Z;

// Z coordinate of front surface
FRONT_Z = PIR_BASE_Z + FRONT_WALL_THICKNESS;

// Wall clearance in mm
WALL_CLEARANCE = 0.4; // voorgestelde proefpassing

// Radius of cover edges
COVER_SMOOTHER = 1.5;

// Overlap of cover parts
COVER_OVERLAP = 2;

//length of countersunk head screws (M3) to mount the back
ORIGINAL_BACK_SCREW_LENGTH = 16;
BATTERY_EXTRA_DEPTH = BATTERY_Z + 2*BATTERY_CLEARANCE_Z + BATTERY_MOUNT_CLEARANCE;
BACK_SCREW_LENGTH = ORIGINAL_BACK_SCREW_LENGTH + BATTERY_EXTRA_DEPTH; // 25 mm
//head diameter of countersunk head screws (M3)
BACK_SCREW_HEAD_DIA= 5.5 +0.5;
//Thickness of back cover
BACK_Z = BACK_SCREW_LENGTH - FRONT_Z + FRONT_WALL_THICKNESS - COVER_OVERLAP + WALL_CLEARANCE;

BATTERY_SUPPORT_Z = BACK_Z - BATTERY_EXTRA_DEPTH - WALL_THICKNESS;
BATTERY_BOTTOM_Z = BATTERY_SUPPORT_Z + BATTERY_CLEARANCE_Z;
assert(BATTERY_Y + 2*BATTERY_CLEARANCE_XY + 2*BATTERY_WIRE_RUN <= PCB_Y);
assert(BATTERY_WIRE_WIDTH < PCB_X - PCB_EDGE_DIA - M3_HOLE_DIA);
echo("Case width / length", CASE_X+2*WALL_THICKNESS, PCB_Y+2*WALL_THICKNESS);
echo("Rear depth / screw length", BACK_Z, BACK_SCREW_LENGTH);


//Diameter of a circle used to position the holes of the wall mount
WALL_MOUNT_DIA = 45;
//Number of screws used for the wall mount
WALL_MOUNT_HOLE_COUNT = 3;
//Wall mount screws head dia (this is Spax 4.5mm...)
WALL_MOUNT_HEAD_DIA = 8.8 + 0.4;
//Diameter of screw holes for wall mount
WALL_MOUNT_HOLE_DIA = 5;



_BLOCK_Z = PCB_Z + FRONT_Z + (2*WALL_THICKNESS);

module camera(block=false)
{
  z = block ? _BLOCK_Z : LENS_Z + PCB_Z;
  translate([0, -(PCB_Y/2) + LENS_BOTTOM_OFFSET, 0])
    cylinder(h=z, d=LENS_DIA);  
}


module oled(block=false)
{
  delta = block ? OLED_Y - OLED_Y_VISIBLE : 0.0;
  y = block ? OLED_Y_VISIBLE-WALL_CLEARANCE : OLED_Y;
  x = block ? OLED_X-2*WALL_CLEARANCE : OLED_X;
  
  
  z = block ? _BLOCK_Z : OLED_Z+PCB_Z;
  translate([-x/2,-y+(PCB_Y/2)-OLED_TOP_OFFSET-delta,0])
    cube([x, y, z]);  
}

module oled_clips()
{
  height = 2*WALL_THICKNESS;
  width = 0.24*OLED_X;
  z = FRONT_Z - height -0.01;
  t = WALL_THICKNESS;
  top_y = PCB_Y/2-OLED_TOP_OFFSET;
  
  translate([-OLED_X/2,top_y + WALL_CLEARANCE,z]) cube([width, t, height]);
  translate([OLED_X/2-width,top_y + WALL_CLEARANCE,z]) cube([width, t, height]);
  translate([- width/2,top_y - OLED_Y - t - WALL_CLEARANCE,z]) cube([width, t, height]);  
  
}



module pir(block=false)
{
  z = block ? _BLOCK_Z : PIR_BASE_Z+PCB_Z;
  translate([0, -(PCB_Y/2) + PIR_BOTTOM_OFFSET, 0])
    union() {
      cylinder(h=z, d= PIR_DIA);
      translate([0,0,z]) sphere(d=PIR_DIA);
    }
}

module rounded_cube(left_x, top_y, width_x, height_y, thickness_z, edge_dia, smooth=0)
{
  r = edge_dia/2;
  
  z_base = smooth > 0 ?  thickness_z - smooth: thickness_z;
  
  
  hull() {
    for (x = [left_x + r, left_x + width_x - r], y = [top_y - r, top_y - height_y + r]) {
      translate([x,y,0]) cylinder(h=z_base, d=edge_dia);
    }
    
    if (smooth > 0) {
      for (x = [left_x + smooth, left_x + width_x - smooth], y = [top_y - smooth, top_y - height_y + smooth])  
      {      
        translate([x,y,z_base])sphere(r=smooth, center=true);
      }
            
    }    
  }
}


module buttons(block=false)
{
    
  z = block ? _BLOCK_Z : PCB_Z + BUTTON_Z;
  
  all_x = [ (-PCB_X/2) + BUTTON_SIDE_OFFSET, (PCB_X/2) - BUTTON_SIDE_OFFSET - BUTTON_XY ];
  start_y = -(PCB_Y/2) + BUTTON_BOTTOM_OFFSET + BUTTON_XY;
  
  r = BUTTON_EDGE_DIA / 2.0;
  
  for (start_x = all_x) {
    rounded_cube(start_x, start_y, BUTTON_XY, BUTTON_XY, z, BUTTON_EDGE_DIA);       
  }  
}

module usb(block=false)
{
  y_offset = block ? (2*WALL_THICKNESS) : 0;
  translate([-USB_X/2, -PCB_Y/2 + USB_BOTTOM_OFFSET - y_offset])
    cube([USB_X, USB_Y + y_offset, USB_Z+PCB_Z]);  
}


module pcb(block=false) 
{
  clearance = block ? WALL_CLEARANCE : 0.0;
  
  // positions of edge holes
  x_off = (PCB_X / 2) - (PCB_EDGE_DIA/2) + clearance;
  y_off = (PCB_Y / 2) - (PCB_EDGE_DIA/2) + clearance;
  
  pcb_height = block ? (FRONT_Z - FRONT_WALL_THICKNESS) : PCB_Z;
 
  // PCB with mount holes
  difference() {
  
    // PCB base    
    rounded_cube(-PCB_X/2-clearance, PCB_Y/2+clearance, PCB_X+(2*clearance), PCB_Y+(2*clearance), pcb_height, PCB_EDGE_DIA);
    
    if (block==false) {
      // mount holes
      union() {
        for (x = [x_off, -x_off], y = [y_off, -y_off]) {
          translate([x,y,0])cylinder(h=4*PCB_Z, d=PCB_MOUNT_HOLE_DIA, center=true);   
        }      
      }
    }
  }
    
}

module espcam(block=false) 
{
  pcb(block);
  oled(block);
  camera(block);
  pir(block);
  buttons(block);
  usb(block);
}

module pcb_edge_mount()
{
  
  // positions of edge holes
  x_off = (PCB_X / 2) - (PCB_EDGE_DIA/2);// + WALL_CLEARANCE;
  y_off = (PCB_Y / 2) - (PCB_EDGE_DIA/2);// + WALL_CLEARANCE;
  
  z = PIR_BASE_Z-WALL_CLEARANCE;
  
  // mount holes
  union() {
    for (x = [x_off, -x_off], y = [y_off, -y_off]) {
      translate([x,y,z/2+WALL_CLEARANCE+PCB_Z]) {
        difference() {
          cylinder(h=z, d=PCB_EDGE_DIA+3*WALL_CLEARANCE, center=true);   
          cylinder(h=4*z, d=M3_TAP_DIA, center=true);
        }
      }
    }      
  }
  
}

module front_cover()
{ 
  difference() {
    
   rounded_cube(-CASE_X/2-WALL_THICKNESS, PCB_Y/2+WALL_THICKNESS, CASE_X + (2*WALL_THICKNESS) ,PCB_Y + (2*WALL_THICKNESS), FRONT_Z, PCB_EDGE_DIA,COVER_SMOOTHER);
      
    
    translate([0,0,-0.01]) espcam(true);
  }
  oled_clips();
  pcb_edge_mount();
    
}

MOUNT_BALL_DIA = PCB_X-(4*WALL_THICKNESS);

module mount_ball()
{
  sphere(d=MOUNT_BALL_DIA, center=true);
}

module m3nut() {
  
   cylinder(h=M3_NUT_HEIGHT * (1.04), d=M3_NUT_DIA * (1.04), $fn=6, center=true);
}

module m3nutrod(h)
{
  hull() {
    translate([0,0,-h/2 + M3_NUT_HEIGHT/2]) m3nut();
    translate([0,0,h/2 - M3_NUT_HEIGHT/2]) m3nut();
  }
}


function sinr(x) = sin(180 * x / PI);
function cosr(x) = cos(180 * x / PI);

module wall_mount_holes(h)
{
  
  n = WALL_MOUNT_HOLE_COUNT;
  dia = WALL_MOUNT_DIA;
  r = dia/2;
  
  delta = (2*PI)/n;
  
  for (step = [0:n-1]) {
    translate([r * cosr(step*delta), r * sinr(step*delta), 0]) {
      cylinder(d=WALL_MOUNT_HOLE_DIA, h=h); 
    
      cylinder(d1=WALL_MOUNT_HEAD_DIA, d2=0, h=WALL_MOUNT_HEAD_DIA/2);
    }
  }
  
}

module wall_mount()
{
  
  d=WALL_MOUNT_DIA+2*WALL_MOUNT_HEAD_DIA;
  h=WALL_MOUNT_HEAD_DIA*0.6;
  z=MOUNT_BALL_DIA;
  translate([0,0,z]) {
    difference() {
      cylinder(d1=d, d2=d+2*WALL_THICKNESS, h=h);
      translate([0,0,-0.1])wall_mount_holes(3*h);
    }
    
    /*hull() {
    //union() {
      
      cylinder(d=0.5*MOUNT_BALL_DIA, h=0.1);
      //translate([0,0,-z/2]) cylinder(d=0.2*MOUNT_BALL_DIA, h=0.1);
      translate([0,0,-z]) cylinder(d=0.5*MOUNT_BALL_DIA, h=0.1);
      
    }*/
    rotate([0,180,0]) {
      cylinder(d1=MOUNT_BALL_DIA, d2=0.4*MOUNT_BALL_DIA, h=0.7*z);
      //cylinder(d=0.4*MOUNT_BALL_DIA,h=z);
    }
    translate([0,0,-z]) sphere(d = MOUNT_BALL_DIA);
    //cylinder(20,20,10,$fn=4);
  }
}

BALL_COVER_DIA = MOUNT_BALL_DIA+2*WALL_THICKNESS;

module back_mount() 
{
  
  DIFF_Z = 10*BALL_COVER_DIA;
  
  
  difference() {
  
    hull()
    //union()
    {
    cylinder(h=2*WALL_THICKNESS, d1=PCB_X, d2=BALL_COVER_DIA);
    
    //cylinder(h=2*WALL_THICKNESS+BALL_COVER_DIA, d=BALL_COVER_DIA);
    translate([0,0,2*WALL_THICKNESS+BALL_COVER_DIA]) sphere(d=BALL_COVER_DIA);
    }
    
    translate([0,0,DIFF_Z/2+2*WALL_THICKNESS]) {
      cylinder(h=DIFF_Z, d=MOUNT_BALL_DIA-WALL_THICKNESS);
      cube([0.5*MOUNT_BALL_DIA, PCB_Y, DIFF_Z], center=true);
    }
    
    translate([0,0,2*WALL_THICKNESS+BALL_COVER_DIA]) sphere(d=MOUNT_BALL_DIA*1.05);
    
    
    cylinder(h=DIFF_Z, d=M3_HOLE_DIA, center=true);
    translate([0,0,2*WALL_THICKNESS-0.5*M3_NUT_HEIGHT]) m3nut();
    
    //hole for M3 screw and nut
    translate([0,0,BALL_COVER_DIA/2+WALL_THICKNESS])rotate([0,90,0]) {
      cylinder(h=DIFF_Z,d=M3_HOLE_DIA,center=true);
      translate([0,0,0.25*MOUNT_BALL_DIA+DIFF_Z/2+2*WALL_THICKNESS]) cylinder(h=DIFF_Z, d=M3_HEAD_DIA,center=true);
      translate([0,0,-0.255*MOUNT_BALL_DIA-DIFF_Z/2-2*WALL_THICKNESS]) m3nutrod(h=DIFF_Z);
    }
    
    translate([0,0,2*WALL_THICKNESS+BALL_COVER_DIA+BALL_COVER_DIA/2]) sphere(d=MOUNT_BALL_DIA*1.1);
    
      
  }
  
  
  %translate([0,0,2*WALL_THICKNESS+BALL_COVER_DIA]) sphere(d=MOUNT_BALL_DIA);
  
  
  
  
}

module back_cover()
{
  //BACK_Z;
  C = WALL_CLEARANCE;
  T = WALL_THICKNESS;
  
  // positions of edge holes
  x_off = (PCB_X / 2) - (PCB_EDGE_DIA/2);// + WALL_CLEARANCE;
  y_off = (PCB_Y / 2) - (PCB_EDGE_DIA/2);// + WALL_CLEARANCE;
  // give some more room to not crush the PCB... 
  FIX_CLEAR = 1*WALL_CLEARANCE;
  z = BACK_Z + COVER_OVERLAP - FIX_CLEAR; //PIR_BASE_Z-WALL_CLEARANCE;
    
  difference() {
    union()
    {   
      //shell
      difference() {
        rounded_cube(-CASE_X/2-T, PCB_Y/2+T, CASE_X + (2*T) ,PCB_Y + (2*T), BACK_Z, PCB_EDGE_DIA,COVER_SMOOTHER);
          
        translate([0,0,-0.01]) rounded_cube(-CASE_X/2-C, PCB_Y/2+C, CASE_X+(2*C) ,PCB_Y+(2*C), BACK_Z-T, PCB_EDGE_DIA,COVER_SMOOTHER);
        
        for (x = [x_off, -x_off], y = [y_off, -y_off]) {
          translate([x,y,z/2+FIX_CLEAR]) {
            cylinder(h=4*z, d=M3_HOLE_DIA, center=true);          
          }
        } 
       //center hole to attach the ball mount later
       cylinder(h=4*z, d=M3_HOLE_DIA, center=true);
        
      }
      // Battery edge supports, attached to the wider side walls.
      for (side=[-1,1]) {
        rail_x = CASE_X/2 + C;
        translate([side > 0 ? rail_x-1.8 : -rail_x-0.1,
                   -BATTERY_Y/2+1, BATTERY_SUPPORT_Z-1.2])
          cube([1.9, BATTERY_Y-2, 1.2]);
      }
      //overlap
      //left_x, top_y, width_x, height_y, thickness_z, edge_dia, smooth=0
      //T = WALL_THICKNESS/2;
      W = (T/2) * 0.9;
      translate([0,0,-COVER_OVERLAP+0.01]) difference() {
        rounded_cube(-CASE_X/2-T, PCB_Y/2+T, CASE_X + (2*T) ,PCB_Y + (2*T), COVER_OVERLAP, PCB_EDGE_DIA);
        
        translate([0,0,-0.1])rounded_cube(-CASE_X/2-T+W, PCB_Y/2+T-W, CASE_X + (2*T) - (2*W) ,PCB_Y + (2*T) - (2*W), 2*COVER_OVERLAP, PCB_EDGE_DIA);   
          }      
     
      // PCB fixing edges   
      
      // mount holes
      union() {
        for (x = [x_off, -x_off], y = [y_off, -y_off]) {
          translate([x,y,z/2-COVER_OVERLAP+FIX_CLEAR]) {
            difference() {
              cylinder(h=z, d=PCB_EDGE_DIA+3*WALL_CLEARANCE, center=true);   
              cylinder(h=4*z, d=M3_HOLE_DIA, center=true);
              
            }
          }
        }      
      }   
                  
    }
    
    
    for (x = [x_off, -x_off], y = [y_off, -y_off]) {
      translate([x,y,BACK_Z-BACK_SCREW_HEAD_DIA/2+0.01]) {

       cylinder(h=BACK_SCREW_HEAD_DIA/2,d1=0,d2=BACK_SCREW_HEAD_DIA);
        
      }
    }  
  }
 
    
}


module overlap()
{
  T = WALL_THICKNESS/2;
  translate([0,0,-COVER_OVERLAP+0.01])
  difference() {
    rounded_cube(-CASE_X/2-T, PCB_Y/2+T, CASE_X + (2*T) ,PCB_Y + (2*T), COVER_OVERLAP, PCB_EDGE_DIA);
    translate([0,0,-0.05]) espcam(true);
  }
}

// Preview only: battery envelope and central short-side wire route.
// The remaining rear cavity is open, so the wire can bend toward the PCB.
module battery_preview() {
  color("silver") translate([-BATTERY_X/2,-BATTERY_Y/2,BATTERY_BOTTOM_Z])
    cube([BATTERY_X,BATTERY_Y,BATTERY_Z]);
  color("red") translate([-BATTERY_WIRE_WIDTH/2,BATTERY_Y/2,
                           BATTERY_BOTTOM_Z])
    cube([BATTERY_WIRE_WIDTH,BATTERY_WIRE_RUN,BATTERY_WIRE_HEIGHT]);
}

if ("preview" == TYPE) {
  //color("PaleTurquoise", 0.7) 
  union() {
    front_cover();
    overlap();
    
  }
  %espcam();
  
  ROT = 25;

  translate([0,0,-BACK_Z]) rotate([0,180,0]){
    back_cover();
    %battery_preview();
    //translate([0,0,-(2*WALL_THICKNESS+BALL_COVER_DIA)] 
    translate([0,0,2*WALL_THICKNESS+BALL_COVER_DIA+BACK_Z]){
       //some fancy rotation animation... 
       r1 = $t < 0.5 ? $t*4*ROT : 2*ROT- ($t-0.5)*ROT *4;
       rotate([-ROT+r1,0,0])wall_mount();
    }
      
    translate([0,0,BACK_Z])back_mount();
  }
  
} else if ("front" == TYPE) {
  union() {
    front_cover();
    overlap();    
  }
} else if ("back" == TYPE) {
  back_cover();

} else if ("backmount" == TYPE) {
  
  %translate([0,0,-BACK_Z])back_cover(); 
  back_mount();
} else if ("wallmount" == TYPE) {
  wall_mount();
  
  %translate([0,0,-(2*WALL_THICKNESS+BALL_COVER_DIA)]){
    back_mount();
    translate([0,0,-BACK_Z])back_cover();
  }
  
}



