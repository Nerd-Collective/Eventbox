<?php
// The popup form to allow KW's to inform the Ops team what days they can work from their invite
function tasker_forms_search_form($form, &$form_state){
  $params = drupal_get_query_parameters();
  // dpm($params);
  $s = !empty($params) && isset($params['search']) && $params['search'] != '' ? $params['search'] : t('Search FAQs');
  $form['search'] = array(
    '#type' => 'textfield',
    '#attributes' => array(
      'placeholder' => array($s),
    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#attributes' => array(
      'class' => array('btn btn-ops'),
    ),
  );
  return $form;
}

function tasker_forms_search_form_submit($form, &$form_state){
  $s = $form_state['values']['search'];
  drupal_goto('faq-search', array('query' => array('search' => $s)), 302);
}
