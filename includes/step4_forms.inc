<?php
/**
 * @file
 *
 */

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form().
 */
function cross_manage_crossingblock_assign_cross_num_form($form, $form_state) {

  $mock = array(
    '2604S' => array('2604S','SX','P1/P15','NY5-161 White/Jetwhite4-11-21', array('2605S','P6','P7')),
    '2605S' => array('2605S','SX','P1/P22','NY5-161 White/WC-BC1-1',array('2604S')),
    '2606S' => array('2606S','SX','P2/P16','NY5-161 Brown/Jetblack4-15-3',array('2608S','P4')),
    '2607aS' => array('2607aS','SX','P2/P22','NY5-161 Brown/WC-BC1-1',array('P5')),
    '2608S' => array('2608S','SX','P3/P17','G9345/Jetblack4-11-11',array()),
    '2610bS' => array('2610bS','SX','P24/P11','1533a-15/Higuera-E',array()),
    '2613bS' => array('2613bS','SX','P47/P13','GDR6aa/737-22',array('P12','P13','P15','P17')),
    '2615S' => array('2615S','SX','P20/P15','316-13/Jetwhite4-11-21',array('P31')),
    '2616S' => array('2616S','SX','P26/P15','1441-8/Jetwhite4-11-21',array('P4')),
    '2617S' => array('2617S','SX','P27/P15','Expresso-5/Jetwhite4-11-21',array('P5')),
    '2618S' => array('2618S','SX','P28/P15','1780TF3-1/Jetwhite4-11-21',array('P6','P7')),
    '2619S' => array('2619S','SX','P41/P15','GDR32a-2/Jetwhite4-11-21',array('P12','P13')),
    '2620bS' => array('2620bS','SX','P20/P16','316-13/Jetblack4-15-3',array('P31')),
    '2621S' => array('2621S','SX','P26/P16','1441-8/Jetblack4-15-3',array('2622S')),
    '2622S' => array('2622S','SX','P27/P16','Expresso-5/Jetblack4-15-3',array('P4','P5'))
  );

  $form['crosses'] = array(
    '#type' => 'fieldset',
    '#title' => 'Crosses',
  );


  return $form;
}

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_assign_cross_num_form_submit($form, $form_state) {

}
