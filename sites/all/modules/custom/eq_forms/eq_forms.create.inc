<?php

/**
 * @file
 * Provides form functionality to create events.
 */

/**
 * Implements hook_form().
 * Provides functionality to create an event.
 */
function eq_forms_create_form($form, &$form_state) {
  global $user;

  if (arg(1) == 'edit') {
    if (is_numeric(arg(2))) {
      $nw = entity_metadata_wrapper('node', arg(2));
      if ($nw->value()) {
        $title = $nw->label();
        $client = $nw->field_e_client->value() ? $nw->field_e_client->getIdentifier() : FALSE;
        $body = $nw->body->value() ? $nw->body->value()['value'] : FALSE;
        $date = $nw->field_e_date->value() ? format_date($nw->field_e_date->value(), 'custom', 'Y-m-d H:i:s') : FALSE;
        $file = $nw->field_e_image->value() ? $nw->field_e_image->value()['fid'] : FALSE;
        $dependent_locality = $nw->field_e_address->value()['dependent_locality'] ? $nw->field_e_address->value()['dependent_locality'] : FALSE;
        $thoroughfare = $nw->field_e_address->value()['thoroughfare'] ? $nw->field_e_address->value()['thoroughfare'] : FALSE;
        $premise = $nw->field_e_address->value()['premise'] ? $nw->field_e_address->value()['premise'] : FALSE;
        $locality = $nw->field_e_address->value()['locality'] ? $nw->field_e_address->value()['locality'] : FALSE;
        $administrative_area = $nw->field_e_address->value()['administrative_area'] ? $nw->field_e_address->value()['administrative_area'] : FALSE;
        $postal_code = $nw->field_e_address->value()['postal_code'] ? $nw->field_e_address->value()['postal_code'] : FALSE;
        $country = $nw->field_e_address->value()['country'] ? $nw->field_e_address->value()['country'] : 'AU';
      }
    }
  }

  $form['wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create Event'),
  );

  // Grab a list of clients created by global user - or 'admin user' of group.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'client')
    ->entityCondition('bundle', 'client')
    ->propertyCondition('uid', $user->uid)
    ->propertyOrderBy('title', 'ASC');
  $results = $query->execute();

  if (count($results['client'])) {
    $clients = array();
    foreach ($results['client'] as $key => $tid) {
      $ew = entity_metadata_wrapper('client', $key);
      $clients[$ew->getIdentifier()] = $ew->label();
    }
  }

  $form['wrapper']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Event Title'),
    '#default_value' => isset($title) ? $title : FALSE,
    '#required' => TRUE,
  );
  $form['wrapper']['client'] = array(
    '#type' => 'textfield',
    '#title' => t('Client'),
    '#description' => t('Create a new client or select from your list.'),
    '#default_value' => isset($client) ? $client : FALSE,
  );

  if (isset($clients)) {
    $form['wrapper']['client_dd'] = array(
      '#type' => 'select',
      '#title' => t('Client'),
      '#options' => $clients,
      '#default_value' => isset($client) ? $client : FALSE,
      '#description' => t('Select a previous client.'),
    );
  }

  // https://www.drupal.org/node/970048
  $form['wrapper']['location'] = array (
    '#type' => 'addressfield',
    '#title' => t('Location'),
    '#required' => FALSE,
    '#default_widget' => 'addressfield_standard',
    '#default_formatter' => 'addressfield_default',
    '#default_value' => array (
      'dependent_locality ' => isset($dependent_locality) ? $dependent_locality : FALSE,
      'thoroughfare' =>  isset($thoroughfare) ? $thoroughfare : FALSE,
      'premise' => isset($premise) ? $premise : FALSE,
      'locality' => isset($locality) ? $locality : FALSE,
      'administrative_area' => isset($administrative_area) ? $administrative_area : FALSE,
      'postal_code' => isset($postal_code) ? $postal_code : FALSE,
      'country' => isset($country) ? $country : 'AU',
    ),
    '#available_countries' => array('AU', 'GB', 'US', 'CA'),
  );
  $form['wrapper']['date'] = array(
    '#type' => 'date_popup',
    '#title' => 'Date',
    '#default_value' => isset($date) ? $date : FALSE,
    '#date_format' => 'd/m/Y',
    '#date_increment' => 30,
    '#date_label_position' => 'within',
    '#date_year_range' => '0:+1',
  );
  $form['wrapper']['notes'] = array(
    '#type' => 'textarea',
    '#title' => t('Notes'),
    '#default_value' => isset($body) ? $body : FALSE,
    '#description' => t('A brief description or some notes?'),
  );
  $form['wrapper']['img'] = array(
    '#type' => 'managed_file',
    '#default_value' => isset($file) ? $file : FALSE,
    '#title' => t('Attachments'),
    '#description' => t('Upload an event image'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('jpg jpeg png'),
      'file_validate_size' => array(5000000),
    ),
    '#upload_location' => 'public://',
  );

  $form['wrapper']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create!'),
    // '#disabled' => sizeof($nw->field_t_users->value()) <= $fbk_size ? true : FALSE,
    '#attributes' => array(
      // 'class' => !isset($requested) ? array('btn-success') : array('btn-danger'),
      'class' => array('btn-success'),
      'data-loading-text' => array("Save.."),
      'data-complete-text' => array('Saved!'),
    ),
  );
  return $form;
}

/**
 * Custom submit function for document_uploader_entityform_edit_form.
 */
function eq_forms_create_form_submit(&$form, &$form_state){
  global $user;

  $fs = $form_state['values'];
  $node = new stdClass();
  $node->type = "event";
  $node->create = strtotime('now');
  $node->language = LANGUAGE_NONE;
  $node->uid = $user->uid;
  $node->status = 1;
  $node->title = $fs['title'];

  // Client.
  if(!empty($fs['client'])){
    // Make new client entity.
    $ew = entity_create('client', array('type' => 'client'));
    $ew->title = strtolower($fs['client']);
    $ew->created = strtotime('now');
    $ew->changed = strtotime('now');
    $ew->uid = $user->uid;
    $ew->save();
    $node->field_e_client[LANGUAGE_NONE][0]['target_id'] = $ew->id;
  }

  // Body.
  if(!empty($fs['notes'])){
    $node->body[LANGUAGE_NONE][0]['value'] = $fs['notes'];
  }

  // Date.
  if(!empty($fs['date'])){
    $node->field_e_date[LANGUAGE_NONE][0]['value'] = format_date(strtotime($fs['date']), 'custom', 'Y-m-d H:i:s');
  }

  // Location.
  if(!empty($fs['location'])){
    $node->field_e_address[LANGUAGE_NONE][0]['thoroughfare'] = $fs['location']['thoroughfare'] ? $fs['location']['thoroughfare'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['premise'] = $fs['location']['premise'] ? $fs['location']['premise'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['dependent_locality'] = $fs['location']['dependent_locality'] ? $fs['location']['dependent_locality'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['locality'] = $fs['location']['locality'] ? $fs['location']['locality'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['administrative_area'] = $fs['location']['administrative_area'] ? $fs['location']['administrative_area'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['postal_code'] = $fs['location']['postal_code'] ? $fs['location']['postal_code'] : FALSE;
    $node->field_e_address[LANGUAGE_NONE][0]['country'] = $fs['location']['country'] ? $fs['location']['country'] : FALSE;
  }
  // File.
  if(!empty($fs['img'])){
    $file = file_load($fs['img']);
    $node->field_e_image = array (
      LANGUAGE_NONE => array (
        0 => array (
          'fid' => $file->fid,
          'filename' => $file->filename,
          'filemime' => $file->filemime,
          'uid' => 1,
          'uri' => $file->uri,
          'status' => 1,
          'display' => 1
        )
      )
    );
  }

  node_save($node);
}

