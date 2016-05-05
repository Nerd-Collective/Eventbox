<?php
// The popup form to allow KW's to inform the Ops team what days they can work from their invite
function tasker_forms_feedback_form($form, &$form_state){
  global $user;
  $nw = entity_metadata_wrapper('node', arg(1));
  $form_state['nid'] = arg(1);
  $usrs = false;

  $desc = t('Provide a rating and feedback for each individual who completed your task:');
  $form['form_description'] = array(
    '#markup' => '<p>'.$desc.'</p>',
  );

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'feedback')
    ->entityCondition('bundle', 'feedback')
    ->propertyCondition('title', '_nid_'.$nw->getIdentifier(), 'CONTAINS');
  $result = $query->execute();
  $fbk_size = !empty($result) ? sizeof($result['feedback']) : 0;
  if(!empty($result)){
    $form_state['eck_result'] = $result;
  }
  $fbk = $fbk_size ? true : false;

  foreach ($nw->field_t_users->getIterator() as $k => $u) {
    $ew = false;
    if($fbk) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'feedback')
        ->entityCondition('bundle', 'feedback')
        ->propertyCondition('title', $u->label().'_uid_'.$u->getIdentifier().'_nid_'.$nw->getIdentifier().'_rating', '=');
      $result = $query->execute();
      if (!empty($result)){
        $ew = entity_metadata_wrapper('feedback', reset($result['feedback'])->id);
      }
    }
    $form['rating_'.$u->getIdentifier()] = array(
      '#type' => 'select',
      '#title' => $u->label(). ': ',
      '#options' => array(0=>'- please select -', 1=>1, 2=>2, 3=>3, 4=>4, 5=>5),
      '#chosen' => true,
      '#default_value' => !empty($ew) ? $ew->field_rating->value() : false,
      '#disabled' => !empty($ew) ? true : false,
      '#description' => t('Give a rating out of 5'),
    );
    $form['msg_'.$u->getIdentifier()] = array(
      '#type' => 'textarea',
      '#default_value' => !empty($ew) ? $ew->field_feedback->value() : false,
      '#disabled' => !empty($ew) ? true : false,
      '#maxlength' => 230,
      '#maxlength_js' => true,
      '#rows' => 3,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('save'),
    '#disabled' => sizeof($nw->field_t_users->value()) <= $fbk_size ? true : false,
    '#attributes' => array(
      'class' => !isset($requested) ? array('btn-success') : array('btn-danger'),
      'data-loading-text' => array("Save.."),
      'data-complete-text' => array('Saved!'),
    ),
  );
  return $form;
}

function tasker_forms_feedback_form_submit($form, &$form_state){
  global $user;
  $nw = entity_metadata_wrapper('node', $form_state['nid']);
  foreach ($nw->field_t_users->getIterator() as $key => $u) {

    if($form_state['values']['rating_'.$u->getIdentifier()] != 0){
      $title = $u->label().'_uid_'.$u->getIdentifier().'_nid_'.$nw->getIdentifier().'_rating';

      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'feedback')
        ->entityCondition('bundle', 'feedback')
        ->propertyCondition('title', $title, '=');
      $result = $query->execute();
      if(!$result){
        $rw = entity_create('feedback', array('type'=>'feedback'));
        $rw->title = $title;
        $rw->created = strtotime('now');
        // $rw->changed = strtotime('now');
        $rw->field_rating[LANGUAGE_NONE][0]['value'] = $form_state['values']['rating_'.$u->getIdentifier()];
        $rw->field_feedback[LANGUAGE_NONE][0]['value'] = $form_state['values']['msg_'.$u->getIdentifier()];
        $rw->field_task_nid[LANGUAGE_NONE][0]['nid'] = $nw->getIdentifier();
        $rw->field_user[LANGUAGE_NONE][0]['uid'] = $u->getIdentifier();
        $rw->save();
      }

      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'feedback')
        ->entityCondition('bundle', 'feedback')
        ->propertyCondition('title', $u->label().'_uid_'.$u->getIdentifier(), 'CONTAINS');
      $result = $query->execute();
      if(!empty($result)){
        $total_rating = 0;
        $total = sizeof($result['feedback']) * 5;
        foreach ($result['feedback'] as $key => $value) {
          $ew = entity_metadata_wrapper('feedback', $value->id);
          $total_rating += $ew->field_rating->value();
        }
        $rating_average = $total_rating / $total * 100;
        $u->field_my_rating->set($rating_average);
        $u->save();
      }
    }
  }

}
