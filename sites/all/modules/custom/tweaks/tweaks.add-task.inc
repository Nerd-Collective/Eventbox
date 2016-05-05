<?php

function tweaks_addtask(){
  $form = drupal_get_form('tweaks_task_add_form');
  return drupal_render($form);
}

// The popup form to allow KW's to inform the Ops team what days they can work from their invite
function tweaks_task_add_form($form, &$form_state){
  if(arg(1) && is_numeric(arg(1))){
    $nw = entity_metadata_wrapper('node', arg(1));
    $form_state['nid_update'] = $nw->getIdentifier();
  }

  if(isset($nw)){
    $cats = array();
    $edit_date = array();
    if($nw->field_t_category->value()){
      foreach ($nw->field_t_category->getIterator() as $key => $cat) {
        $cats[$cat->value()] = $cat->value();
      }
    }
    if($nw->field_t_date->value()){
      $edit_date = format_date($nw->field_t_date->value(), 'custom', 'Y-m-d H:i:s');
    }
  }

  if(isset($nw)){
    $field_status = field_info_field('field_t_status');
    $allowed_values = list_allowed_values($field_status);

    $form['wrapper_status'] = array(
      '#type' => 'fieldset',
      '#title' => 'Update Status',
    );
    $form['wrapper_status']['status'] = array(
      '#type' => 'select',
      '#chosen' => true,
      '#title' => 'Task Status',
      '#options' => $allowed_values,
      '#default_value' => isset($nw) ? $nw->field_t_status->value() : false,
      '#description' => "This is the current status of your task.",
    );
  }

  $form['wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => isset($nw) ? t('Update Task') : t('Add Task'),
  );
  $form['wrapper']['title'] = array(
    '#type' => 'textfield',
    '#title' => 'Task',
    '#default_value' => isset($nw) ? $nw->label() : false,
  );
  $form['wrapper']['description'] = array(
    '#type' => 'textarea',
    '#title' => 'Description',
    '#default_value' => isset($nw) ? $nw->body->value()['value'] : false,
    '#description' => "Provide a brief description of your task.",
  );
  // $points = drupal_map_assoc(array(5,10,25,50,100));
  // $form['wrapper']['points'] = array(
  //   '#type' => 'select',
  //   '#title' => 'Points',
  //   '#chosen' => true,
  //   '#options' => $points,
  //   '#default_value' => isset($nw) ? $nw->field_t_points->value() : false,
  //   '#description' => "Points for completing your task.",
  // );
  $difficulty = drupal_map_assoc(array(1,2,3,4,5));
  $form['wrapper']['diff'] = array(
    '#type' => 'select',
    '#title' => 'Difficulty',
    '#chosen' => true,
    '#options' => $difficulty,
    '#default_value' => isset($nw) ? $nw->field_t_difficulty->value() : false,
    '#description' => "Difficulty ranging from low to high.",
  );

  $field_cat = field_info_field('field_t_category');
  $allowed_values = list_allowed_values($field_cat);

  $form['wrapper']['cat'] = array(
    '#type' => 'select',
    '#title' => 'Category',
    '#chosen' => true,
    '#multiple' => true,
    '#options' => $allowed_values,
    '#default_value' => isset($nw) ? $cats : false,
    '#description' => "Task category.",
  );

  $form['wrapper']['date'] = array(
    '#type' => 'date_popup',
    '#title' => 'Start Date',
    // '#default_value' => ( isset($existing_data) && $existing_data[$i]['date'] ) ? $existing_data[$i]['date'] : $start, //check that out: date in ISO format
    '#date_format' => 'd/m/Y',
    '#date_increment' => 30,
    '#date_label_position' => 'within',
    '#date_year_range' => '0:+1',
    '#description' => 'If you have a date for the task?',
    '#default_value' => isset($edit_date) ? $edit_date : false,
    '#datepicker_options' => array(),
  );

  $field_loc = field_info_field('field_t_location');
  $allowed_values = list_allowed_values($field_loc);
  $form['wrapper']['location'] = array(
    '#type' => 'select',
    '#title' => 'Location',
    '#chosen' => true,
    '#options' => $allowed_values,
    '#default_value' => isset($nw) ? $nw->field_t_location->value() : false,
    '#description' => 'The main location.',
  );

  // Set the times regarding the units..
  if( !empty($form_state['values']['time_unit']) && $form_state['values']['time_unit'] == 'd' ){
    $times = array(1=>1,2=>2,3=>3,4=>4,5=>5,6=>6,7=>7,8=>8,9=>9,10=>10,11=>11,12=>12,13=>13,14=>14);
  } else {
    $times = array(1=>1,2=>2,3=>3,4=>4,5=>5,6=>6,7=>7,8=>8,9=>9,10=>10,11=>11,12=>12,13=>13,14=>14);
  }
  $form['wrapper']['time'] = array(
    '#type' => 'select',
    '#title' => 'Time',
    '#chosen' => true,
    '#options' => $times,
    '#default_value' => isset($nw) ? $nw->field_t_time->value() : false,
    '#description' => 'How long the task will take - select the unit below.',
    '#prefix' => '<div id="time-formatter">',
    '#suffix' => '</div>',
  );

  // $form['wrapper']['time_unit'] = array(
  //   '#type' => 'select',
  //   '#title' => 'Time unit',
  //   '#chosen' => true,
  //   '#options' => array('h'=>'Hours', 'd'=>'Days'),
  //   '#default_value' => isset($nw) ? $nw->field_t_time_unit->value() : false,
  //   '#description' => "Time required to complete task (hours/days).",
  //   '#ajax' => array(
  //     'callback' => 'tweaks_ajax_callback',
  //     'wrapper' => 'time-formatter',
  //   ),
  // );

  $form['wrapper']['submit'] = array(
    '#type' => 'submit',
    '#default_value' => isset($nw) ? t('update task') : t('add task'),
    '#attributes' => array(
      'class' => array('btn-success'),
      'data-loading-text' => array("Saving.."),
      'data-complete-text' => array('Saved!'),
    ),
    '#prefix' => '<div class="form-item">',
    '#suffix' => '</div>',
  );
  return $form;
}

function tweaks_task_add_form_validate($form, &$form_state){
  if ($form_state['values']['title'] == 'test') {
    form_set_error('title', 'Title cannot be "test"');
    form_set_error('date', 'Title cannot be "test"');
  }
  if(empty($form_state['values']['title']) || $form_state['values']['title'] == ' '){
    form_set_error('title', t('oops- no title!'));
  }
}
function tweaks_task_add_form_submit($form, &$form_state){
  global $user;

  if(!isset($form_state['nid_update'])){
    $node = new stdClass();
    $node->type = "task";
    node_object_prepare($node);
    drupal_set_message(t('Good news! Your task: '.$form_state['values']['title'].' has been submitted! Check your '.l('created tasks', 'tasks-created').' queue to see the status.'), 'status', FALSE);
    $node->field_t_status[LANGUAGE_NONE][0]['value'] = 'created';
  } else {
    drupal_set_message(t('Task: '.$form_state['values']['title'].' has been updated!'), 'status', FALSE);
    $node = node_load($form_state['nid_update']);
    $node->field_t_status[LANGUAGE_NONE][0]['value'] = $form_state['values']['status'];
  }

  $node->title = $form_state['values']['title'];
  $node->language = LANGUAGE_NONE;
  $node->uid = $user->uid;
  $node->body[LANGUAGE_NONE][0]['value'] = $form_state['values']['description'];

  $i=0;
  foreach ($form_state['values']['cat'] as $k => $v) {
    $node->field_t_category[LANGUAGE_NONE][$i]['value'] = $k;
    $i++;
  }
  $node->field_t_points[LANGUAGE_NONE][0]['value'] = td_create_points($form_state['values']['time'], $form_state['values']['diff']);
  if(!empty($form_state['values']['date'])){
    $node->field_t_date[LANGUAGE_NONE][0]['value'] = format_date(strtotime($form_state['values']['date']), 'custom', 'Y-m-d H:i:s');
  }
  $node->field_t_location[LANGUAGE_NONE][0]['value'] = $form_state['values']['location'];
  $node->field_t_time[LANGUAGE_NONE][0]['value'] = $form_state['values']['time'];
  $node->field_t_time_unit[LANGUAGE_NONE][0]['value'] = 'h';
  $node->field_t_difficulty[LANGUAGE_NONE][0]['value'] = $form_state['values']['diff'];
  node_save($node);
  drupal_goto('node/'.$node->nid);
}

function tweaks_ajax_callback($form, $form_state) {
  return $form['wrapper']['time'];
}
